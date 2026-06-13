#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <assert.h>

#include <stddef.h>
#include <stdint.h>

#include "logging.h"
#include "song.h"

const int screenWidth  = 1280;
const int screenHeight = 720;

#define UNUSED(v) (void) (v)

void DrawNote(int x, int y, int width, float amplitude, int max_height);

// https://www.raylib.com/examples/audio/loader.html?name=audio_sound_loading
// #define MAX_DB (70)
// #define MIN_DB (0)
// float amplitude_to_db(float A) { return (20 * log10f(A)); }

static size_t current_sample;

typedef float (*function_sampler_t)(float x);

typedef struct {
    Rectangle          rect;
    int                padding;
    Color              color;
    function_sampler_t sampler;
} graph_t;

graph_t graph_create(int x, int y, int width, int height, int padding, function_sampler_t sampler) {
    graph_t graph;

    graph.rect.x      = x + padding;
    graph.rect.y      = y + padding;
    graph.rect.width  = width - 2 * padding;
    graph.rect.height = height - 2 * padding;
    graph.color       = PURPLE;

    graph.sampler = sampler;

    return graph;
}

void graph_draw(graph_t *graph) {
    if (graph == NULL) {
        return;
    }

    // Draw a rectangle representing the graph
    DrawRectangleRec(graph->rect, graph->color);

    Vector2 last_point = {
        graph->rect.x,
        graph->sampler(0) * graph->rect.height + graph->rect.y,
    };
    for (int i = 1; i < graph->rect.width; i++) {
        float   x     = (float) i / graph->rect.width;  // Normalize x to [0, 1]
        float   y     = graph->sampler(x);              // Sample the function
        Vector2 point = {
            x * graph->rect.width + graph->rect.x,
            y * graph->rect.height + graph->rect.y,
        };
        DrawLineV(last_point, point, BLACK);
        last_point = point;
    }
}

float note(float frequency, float amplitude, float x) {
    return amplitude * (0.5f + 0.5f * sinf(2 * PI * x * frequency));
}

float test_sampler(float x) {
    return note(10.0f, 1.0f, x);
}

#include <stdio.h>
#include <assert.h>

#define AUDIO_SRC "./audio/Anton_Reicha_Fugue_24.wav"

void print_wave_info(Wave wav) {
    printf("\tFrame Count: '%d'\n", wav.frameCount);
    printf("\tSample Rate: '%d'\n", wav.sampleRate);
    printf("\tSample Size: '%d'\n", wav.sampleSize);
    printf("\tChannels: '%d'\n", wav.channels);
}

#define MAX_SAMPLE_COUNT (screenWidth)

#define SLIDER_H       7
#define SLIDER_THUMB_R 10

int ui_slider(int x, int y, int width, int max_val, int *position) {
    int changed = 0;

    Rectangle track = {(float) x, (float) (y - SLIDER_H / 2), (float) width, (float) SLIDER_H};

    float t        = (max_val > 0) ? (float) *position / (float) max_val : 0.0f;
    float thumb_cx = x + t * width;
    float thumb_cy = (float) y;

    Vector2 mouse      = GetMousePosition();
    bool    over_thumb = CheckCollisionPointCircle(mouse, (Vector2){thumb_cx, thumb_cy}, SLIDER_THUMB_R + 4);

    static int active_y = -1;

    if (over_thumb && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        active_y = y;

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        active_y = -1;

    bool dragging = (active_y == y);

    if (dragging) {
        float raw     = (mouse.x - (float) x) / (float) width;
        float clamped = raw < 0.0f ? 0.0f : raw > 1.0f ? 1.0f : raw;
        int   new_val = (int) (clamped * (float) max_val + 0.5f);
        if (new_val != *position) {
            *position = new_val;
            changed   = 1;
        }
        thumb_cx = x + clamped * width;
    }

    DrawRectangleRounded(track, 1.0f, 6, DARKGRAY);

    Rectangle active = {track.x, track.y, thumb_cx - track.x, track.height};
    DrawRectangleRounded(active, 1.0f, 6, SKYBLUE);

    if (over_thumb || dragging)
        DrawCircleLines((int) thumb_cx, (int) thumb_cy, SLIDER_THUMB_R + 5, Fade(SKYBLUE, 0.4f));

    DrawCircleV((Vector2){thumb_cx, thumb_cy}, SLIDER_THUMB_R, dragging ? BLUE : SKYBLUE);

    return changed;
}

#define CHECKBOX_SIZE 16

int ui_checkbox(int x, int y, bool *checked) {
    Rectangle box   = {(float) x, (float) y, CHECKBOX_SIZE, CHECKBOX_SIZE};
    Vector2   mouse = GetMousePosition();
    bool      over  = CheckCollisionPointRec(mouse, box);

    if (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        *checked = !*checked;
        return 1;
    }

    Color fill   = *checked ? SKYBLUE : DARKGRAY;
    Color border = (over || *checked) ? BLUE : GRAY;
    DrawRectangleRounded(box, 0.2f, 4, fill);
    DrawRectangleRoundedLines(box, 0.2f, 4, border);

    if (*checked) {
        float cx = x + CHECKBOX_SIZE * 0.5f;
        float cy = y + CHECKBOX_SIZE * 0.5f;
        DrawLineEx((Vector2){cx - 5, cy}, (Vector2){cx - 1, cy + 4}, 2.0f, WHITE);
        DrawLineEx((Vector2){cx - 1, cy + 4}, (Vector2){cx + 5, cy - 4}, 2.0f, WHITE);
    }

    return 0;
}

typedef struct SampleBuffer {
    size_t    length;
    size_t    capacity;
    uint16_t *data;
} SampleBuffer;

SampleBuffer sb_create(uint16_t *backing, size_t cap) {
    SampleBuffer sb = {0};
    sb.data         = backing;
    sb.capacity     = cap;
    sb.length       = 0;

    return sb;
}

typedef uint8_t BufferViewKind;
enum {
    BUFFER_VIEW_KIND_SLICE = 0,
};

int VISUAL_SCALING_FACTOR = 50;

void sb_render_slices(SampleBuffer *b, int x, int y, size_t start, size_t length) {
    for (size_t i = 0; i < length; i++) {
        uint16_t sample = b->data[start + i];
        DrawNote(x + i, y, 1, (((float) UINT16_MAX - sample) / UINT16_MAX) * ((float) VISUAL_SCALING_FACTOR / 100),
                 screenHeight);
    }
}

void sb_view(SampleBuffer *b, BufferViewKind kind, int x, int y, size_t start, size_t length) {
    assert(b);

    assert(start <= b->capacity);
    assert(start + length <= b->capacity);

    switch (kind) {
        case BUFFER_VIEW_KIND_SLICE: {
            sb_render_slices(b, x, y, start, length);
            break;
        }
        default: {
            break;
        };
    }
}

static inline void sb_push_sample(SampleBuffer *buf, uint16_t sample) {
    assert(buf->length + 1 <= buf->capacity);

    buf->data[buf->length] = sample;
    buf->length++;
}

// claude: translate this comment to simplified chinese
// static inline void sb_push_sample_mod(SampleBuffer *sb, float value) {
//     sb->data[sb->length % MAX_SAMPLE_COUNT] = value;
//     sb->length                              = (sb->length + 1) % MAX_SAMPLE_COUNT;
// }

// Draw a vertical bar representing the note's amplitude at the given position
// x: The x-coordinate of the bar's left edge
// y: The y-coordinate of the bar's center
// width: The width of the bar
// amplitude: The amplitude of the note (0.0 to 1.0)
// max_height: The maximum height of the bar when amplitude is 1.0
void DrawNote(int x, int y, int width, float amplitude, int max_height) {
    const int height = (int) (amplitude * (float) max_height);
    DrawRectangle(x, y - height / 2, width, height, BLUE);
}

// void sb_slice_graph(SampleBuffer *sb, int x, int y, int slice_width) {
//     for (size_t i = 0; i < sb->length; i++) {
//         uint16_t sample = sb->data[i];
//         DrawNote(x + i * slice_width, y, slice_width, sample / (float) UINT16_MAX, screenHeight);
//     }
// }

static SampleBuffer sampleBuffer = {0};

#define AUDIO_STREAM_SAMPLE_RATE 44100
#define AUDIO_STREAM_SAMPLE_SIZE 16
#define AUDIO_STREAM_CHANNELS    2

// NOTE: we can setup our own audio buffer and just memcpy instead of iterating
// through frames
void fill_audio_buffer(void *buffer, unsigned int frames) {
    if (is_song_finished(MARY_HAD_A_LITTLE_LAMB, (float) current_sample / AUDIO_STREAM_SAMPLE_RATE))
        return;

    for (unsigned int frame = 0; frame < frames; frame++) {
        uint16_t *sample_buffer = (uint16_t *) buffer;

        float sample = sample_song(MARY_HAD_A_LITTLE_LAMB, (float) current_sample / AUDIO_STREAM_SAMPLE_RATE);

        sample_buffer += frame * AUDIO_STREAM_CHANNELS;
        sample_buffer[0] = (uint16_t) (sample * UINT16_MAX);
        sample_buffer[1] = sample_buffer[0];  // Duplicate for stereo

        sb_push_sample(&sampleBuffer, sample_buffer[0]);

        current_sample++;
    }
}

// NOTE: the below link seems to have info we need to get the Audio Stream working.
// ref:
// https://github.com/raysan5/raylib/blob/4ebe7d62159257cae48d47afd4d8ce2ecf0afb6e/examples/audio/audio_raw_stream.c
int main(void) {
    size_t    BYTES   = (1 << 20) * 16 * sizeof(uint16_t);
    uint16_t *progMem = malloc(BYTES);
    assert(progMem);

    sampleBuffer = sb_create(progMem, BYTES);

    UNUSED(current_sample);

    SetTraceLogCallback(ColouredLog);

    // ---------

    // Initialization
    //--------------------------------------------------------------------------------------
    InitAudioDevice();

    InitWindow(screenWidth, screenHeight, "seawaves");

    TraceLog(LOG_INFO, "Creating graph...");
    // graph_t graph = graph_create(0, 0, screenWidth, screenHeight, 50, test_sampler);

    SetTargetFPS(24);  // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    SetMasterVolume(0.2);
    TraceLog(LOG_ERROR, "Master volume set to %f", GetMasterVolume());

    // --- AUDIO STREAMING ------
    // Wave wave = LoadWave(AUDIO_SRC);
    // assert(IsWaveValid(wave));

    // print_wave_info(wave);

    // float *wave_samples = LoadWaveSamples(wave);
    // assert(wave_samples);

    // printf("samples: %p\n", (void *) wave_samples);

    // SetAudioStreamBufferSizeDefault(DEFAULT_BUFFER_SIZE);

    // AudioStream stream = LoadAudioStream(wave.sampleRate, wave.sampleSize, wave.channels);

    // SetAudioStreamBufferSizeDefault(4096);
    AudioStream stream = LoadAudioStream(AUDIO_STREAM_SAMPLE_RATE, AUDIO_STREAM_SAMPLE_SIZE, AUDIO_STREAM_CHANNELS);
    assert(IsAudioStreamValid(stream));

    SetAudioStreamCallback(stream, fill_audio_buffer);
    PlayAudioStream(stream);

    if (!IsAudioStreamPlaying(stream)) {
        TraceLog(LOG_ERROR, "Audio stream is not playing!");
    } else {
        TraceLog(LOG_INFO, "Audio stream is playing.");
    }

    /*
    if (IsAudioStreamProcessed(stream)) {
        // UpdateAudioStream(stream, samples, 4096);
    }
    */

    // for (size_t i = 0; i < MAX_SAMPLE_COUNT; i++) { sb_push_sample_mod(&sampleBuffer, 0); }
    int  start = 0;
    bool track = true;
    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------
        // UpdateAudioStream(stream, samples, wave.frameCount);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // SCROLL UI
        if (track) {
            start = (sampleBuffer.length < screenWidth) ? 0 : sampleBuffer.length - screenWidth;
        }

        ui_slider(10, 96, 256, 100, &VISUAL_SCALING_FACTOR);
        ui_slider(10, 10, 620, sampleBuffer.length - screenWidth, &start);
        ui_checkbox(700, 10, &track);

        sb_view(&sampleBuffer, BUFFER_VIEW_KIND_SLICE, 0, screenHeight / 2.0, start, screenWidth);
        // graph_draw(&graph);

        // BeginMode2D(camera);

        // DrawCircle(0, 0, 50, RED);
        // DrawText("ZERO", 0, 0, 20, DARKGRAY);
        // DrawText("HALF", screenWidth / 2, screenHeight / 2, 20, DARKGRAY);
        // DrawText("FULL", screenWidth, screenHeight, 20, DARKGRAY);

        // EndMode2D();

        // NOTE:
        // sb_slice_graph(&sampleBuffer, 0, screenHeight / 2, 1);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseAudioDevice();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    free(progMem);

    return 0;
}
