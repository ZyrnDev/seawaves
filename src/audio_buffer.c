#include "audio_buffer.h"

#include <assert.h>
#include <string.h>

AudioBuffer ab_create(void *backing, size_t cap) {
    return (AudioBuffer){
        .cap     = cap,
        .len     = 0,
        .samples = backing,
    };
}

void ab_push_sample(AudioBuffer *ab, AudioSample s) {
    (void) ab;
    (void) s;
}

void audio_buffer_reset(AudioBuffer *ab) {
    assert(ab);
    ab->len = 0;
}
