#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct AudioBuffer {
    float *samples;
    size_t cap;
    size_t len;
} AudioBuffer;

typedef struct AudioSample {
    float frequency;
    float amplitude;
    float duration;
} AudioSample;

AudioBuffer ab_create(void *backing, size_t cap);
void        ab_push_sample(AudioBuffer *ab, AudioSample s);
void        ab_reset(AudioBuffer *ab);

#endif  // AUDIO_BUFFER_H
