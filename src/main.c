#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include "audio_buffer.h"

#include "logging.h"
#include "song.h"

#define UNUSED(v) (void) (v)

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

#define AUDIO_STREAM_SAMPLE_RATE 44100
#define AUDIO_STREAM_SAMPLE_SIZE 16
#define AUDIO_STREAM_CHANNELS    2
// NOTE: we can setup our own audio buffer and just memcpy instead of iterating
// through frames
void fill_audio_buffer(void *buffer, unsigned int frames) {
    for (unsigned int frame = 0; frame < frames; frame++) {
        uint16_t *sample_buffer = (uint16_t *) buffer;

        float sample = sample_song(MARY_HAD_A_LITTLE_LAMB, (float) current_sample / AUDIO_STREAM_SAMPLE_RATE);

        sample_buffer += frame * AUDIO_STREAM_CHANNELS;
        sample_buffer[0] = (uint16_t) (sample * UINT16_MAX);
        sample_buffer[1] = sample_buffer[0];  // Duplicate for stereo

        current_sample++;
    }

    // for (unsigned int frame = 0; frame < frames; frame++) {
    //     uint16_t *sample_ptr = (uint16_t *) buffer;
    //     sample_ptr += frame * AUDIO_STREAM_CHANNELS;
    //     sample_ptr[0] =
    //         (uint16_t) (note(440.0f, (float) UINT16_MAX, (float) current_sample / AUDIO_STREAM_SAMPLE_RATE));
    //     sample_ptr[1] = sample_ptr[0];  // Duplicate for stereo

    //     current_sample = (current_sample + 1) % AUDIO_STREAM_SAMPLE_RATE;
    // }
}

// NOTE: the below link seems to have info we need to get the Audio Stream working.
// ref:
// https://github.com/raysan5/raylib/blob/4ebe7d62159257cae48d47afd4d8ce2ecf0afb6e/examples/audio/audio_raw_stream.c
int main(void) {
    UNUSED(current_sample);

    SetTraceLogCallback(ColouredLog);

    // ---------

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth  = 1280;
    const int screenHeight = 720;

    InitAudioDevice();

    InitWindow(screenWidth, screenHeight, "seawaves");

    TraceLog(LOG_INFO, "Creating graph...");
    graph_t graph = graph_create(0, 0, screenWidth, screenHeight, 50, test_sampler);

    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
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

        graph_draw(&graph);

        // BeginMode2D(camera);

        // DrawCircle(0, 0, 50, RED);
        // DrawText("ZERO", 0, 0, 20, DARKGRAY);
        // DrawText("HALF", screenWidth / 2, screenHeight / 2, 20, DARKGRAY);
        // DrawText("FULL", screenWidth, screenHeight, 20, DARKGRAY);

        // EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseAudioDevice();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
