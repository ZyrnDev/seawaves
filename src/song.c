#include "song.h"

#include <math.h>

#define PI 3.14159265358979323846
float sine_wave(float frequency, float amplitude, float time) {
    return amplitude * (0.5f + 0.5f * sinf(2.0f * PI * time * frequency));
}

// https://en.wikipedia.org/wiki/Sawtooth_wave
float saw_wave(float frequency, float amplitude, float time) {
    return amplitude * (0.5f + 0.5f * fmodf(time * frequency, 1.0));
    // return 2.0f * amplitude * atanf(PI * time * frequency);
}

float square_wave(float frequency, float amplitude, float time) {
    float t = fmodf(time * frequency, 1.0f);
    return amplitude * (0.5f + ((t < 0.5f) ? 0.5f : -0.5f));
}

float triangle_wave(float frequency, float amplitude, float time) {
    float t = fmodf(time * frequency, 1.0f);
    return amplitude * (1.0f - 4.0f * fabs(t - 0.5f));
}

float sample_song(Song song, float time) {
    float sample = 0.0f;

    for (size_t t = 0; t < song.track_count; t++) {
        Track track = song.tracks[t];

        float track_time = 0.0f;
        for (size_t x = 0; x < track.hit_count; x++) {
            Hit hit = track.hits[x];

            if (time >= track_time && time < track_time + hit.duration) {
                sample += square_wave(hit.frequency, hit.amplitude, time - track_time);
            }

            track_time += hit.duration;
        }
    }

    return sample;  // not normalized, just a sum of frequencies for now
}
