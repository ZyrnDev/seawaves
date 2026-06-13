#include "song.h"

#include <math.h>
typedef float (*wave_function_t)(float frequency, float amplitude, float time);

#define PI 3.14159265358979323846

float harmonic_wave(wave_function_t wave_func, size_t harmonics, float harmonic_decay, float frequency, float amplitude,
                    float time) {
    float sample = 0.0f;

    const float total_weight = (1.0f - powf(harmonic_decay, harmonics)) / (1.0f - harmonic_decay);

    for (size_t i = 0; i < harmonics; i++) {
        float harmonic_freq = frequency * (i + 1);
        float harmonic_amp  = amplitude * powf(harmonic_decay, i);
        float weight        = powf(harmonic_decay, i) / total_weight;  // Normalize amplitude
        sample += weight * wave_func(harmonic_freq, harmonic_amp, time);
    }

    return sample;
}

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
    return 0.5f + amplitude * .99 * ((t < 0.5f) ? 0.5f : -0.5f);
}

float triangle_wave(float frequency, float amplitude, float time) {
    float t = fmodf(time * frequency, 1.0f);
    return 0.5f + amplitude * (1.0f - 4.0f * fabsf(t - 0.5f));
    amplitude * (1.0f - 4.0f * fabs(t - 0.5f));
}

float sample_song(Song song, float time) {
    float sample = 0.0f;

    for (size_t t = 0; t < song.track_count; t++) {
        Track track = song.tracks[t];

        float track_time = 0.0f;
        for (size_t x = 0; x < track.hit_count; x++) {
            Hit hit = track.hits[x];

            if (time >= track_time && time < track_time + hit.duration) {
                sample += harmonic_wave(sine_wave, 5, 0.5f, hit.frequency, hit.amplitude, time - track_time);
            }

            track_time += hit.duration;
        }
    }

    return sample;  // not normalized, just a sum of frequencies for now
}

bool is_song_finished(Song song, float time) {
    float total_time = 0.0f;

    for (size_t t = 0; t < song.track_count; t++) {
        Track track = song.tracks[t];

        for (size_t x = 0; x < track.hit_count; x++) {
            Hit hit = track.hits[x];
            total_time += hit.duration;
        }
    }

    return time >= total_time;
}
