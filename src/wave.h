#ifndef __SEAWAVES_WAVE_H__
#define __SEAWAVES_WAVE_H__

#include <stdint.h>
#include <stdio.h>

typedef struct riff_header {
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
} riff_header_t;

typedef struct fmt_subchunk {
    char subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} fmt_subchunk_t;

typedef struct data_subchunk {
    char subchunk2_id[4];
    uint32_t subchunk2_size;
    uint8_t* data;
} data_subchunk_t;

typedef struct wav_file {
    riff_header_t riff_header;
    fmt_subchunk_t fmt_subchunk;
    data_subchunk_t data_subchunk;
} wav_file_t;

wav_file_t* wav_file_create(void);
void wav_file_free(wav_file_t* wav);

void wav_file_print(const wav_file_t* wav);

wav_file_t* wav_file_read(FILE* file);
void wav_file_write(const wav_file_t* wav, FILE* file);

#endif // __SEAWAVES_WAVE_H__
