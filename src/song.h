#ifndef SONG_H
#define SONG_H

#include <stddef.h>

typedef struct {
    float frequency;
    float amplitude;
    float duration;
} Hit;

#define HIT(f, a, d)                                   \
    (Hit) {                                            \
        .frequency = f, .amplitude = a, .duration = d, \
    }

typedef struct {
    size_t hit_count;
    Hit   *hits;
} Track;

typedef struct {
    size_t track_count;
    Track *tracks;
} Song;

float sample_song(Song song, float time);

#define MARY_HAD_A_LITTLE_LAMB                        \
    ((Song) {                                         \
        .track_count = 1,                             \
        .tracks =                                     \
            (Track[]) {                               \
                {                                     \
                    .hit_count = 8,                   \
                    .hits =                           \
                        (Hit[]) {                     \
                            HIT(440.0f, 1.0f, 0.5f),  \
                            HIT(392.0f, 1.0f, 0.5f),  \
                            HIT(349.23f, 1.0f, 0.5f), \
                            HIT(392.0f, 1.0f, 0.5f),  \
                            HIT(440.0f, 1.0f, 0.5f),  \
                            HIT(440.0f, 1.0f, 0.5f),  \
                            HIT(440.0f, 1.0f, 1.0f),  \
                            HIT(392.0f, 1.0f, 2.0f),  \
                        },                            \
                },                                    \
            },                                        \
    })

#endif  // SONG_H
