#include "wave.h"

#include <stdio.h>
#include <stdlib.h>

wav_file_t* wav_file_create(void) {
    wav_file_t* ptr = calloc(1, sizeof(wav_file_t));
    if (ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory for wav_file_t\n");
        return NULL;
    }

    // // Initialize the RIFF header with default values
    // ptr->riff_header.chunk_id[0] = 'R';
    // ptr->riff_header.chunk_id[1] = 'I';
    // ptr->riff_header.chunk_id[2] = 'F';
    // ptr->riff_header.chunk_id[3] = 'F';

    // ptr->riff_header.format[0] = 'W';
    // ptr->riff_header.format[1] = 'A';
    // ptr->riff_header.format[2] = 'V';
    // ptr->riff_header.format[3] = 'E';

    return ptr;
}

void wav_file_free(wav_file_t* wav) {
    if (wav != NULL) {
        if (wav->data_subchunk.data != NULL) {
            free(wav->data_subchunk.data);
            wav->data_subchunk.data = NULL;
        }
        free(wav);
    }
}

void wav_file_print(const wav_file_t* wav) {
    if (wav == NULL) {
        fprintf(stderr, "wav_file pointer is NULL\n");
        return;
    }

    printf("WAV File:\n");
    printf("  RIFF Header:\n");
    printf("    Chunk ID: %.4s\n", wav->riff_header.chunk_id);
    printf("    Chunk Size: %u\n", wav->riff_header.chunk_size);
    printf("    Format: %.4s\n", wav->riff_header.format);
    printf("  Format Subchunk:\n");
    printf("    Subchunk1 ID: %.4s\n", wav->fmt_subchunk.subchunk1_id);
    printf("    Subchunk1 Size: %u\n", wav->fmt_subchunk.subchunk1_size);
    printf("    Audio Format: %u\n", wav->fmt_subchunk.audio_format);
    printf("    Number of Channels: %u\n", wav->fmt_subchunk.num_channels);
    printf("    Sample Rate: %u\n", wav->fmt_subchunk.sample_rate);
    printf("    Byte Rate: %u\n", wav->fmt_subchunk.byte_rate);
    printf("    Block Align: %u\n", wav->fmt_subchunk.block_align);
    printf("    Bits per Sample: %u\n", wav->fmt_subchunk.bits_per_sample);
    printf("  Data Subchunk:\n");
    printf("    Subchunk2 ID: %.4s\n", wav->data_subchunk.subchunk2_id);
    printf("    Subchunk2 Size: %u\n", wav->data_subchunk.subchunk2_size);
    printf("    Data: %p\n", (void*)wav->data_subchunk.data);
}

wav_file_t* wav_file_read(FILE* file) {
    if (file == NULL) {
        fprintf(stderr, "File pointer is NULL\n");
        return NULL;
    }

    wav_file_t* wav = calloc(1, sizeof(wav_file_t));
    if (wav == NULL) {
        fprintf(stderr, "Failed to allocate memory for wav_file_t\n");
        return NULL;
    }

    size_t read_size;

    read_size = fread(&wav->riff_header, sizeof(riff_header_t), 1, file);
    if (read_size != 1) {
        fprintf(stderr, "failed to read RIFF header from file\n");
        wav_file_free(wav);
        return NULL;
    }

    read_size = fread(&wav->fmt_subchunk, sizeof(fmt_subchunk_t), 1, file);
    if (read_size != 1) {
        fprintf(stderr, "failed to read fmt subchunk from file\n");
        wav_file_free(wav);
        return NULL;
    }

    read_size = fread(&wav->data_subchunk, sizeof(data_subchunk_t) - sizeof(((data_subchunk_t *)0)->data), 1, file);
    if (read_size != 1) {
        fprintf(stderr, "failed to read data subchunk from file\n");
        wav_file_free(wav);
        return NULL;
    }

    // Allocate memory for the audio data
    wav->data_subchunk.data = malloc(wav->data_subchunk.subchunk2_size);
    if (wav->data_subchunk.data == NULL) {
        fprintf(stderr, "failed to allocate memory for audio data\n");
        wav_file_free(wav);
        return NULL;
    }

    read_size = fread(wav->data_subchunk.data, 1, wav->data_subchunk.subchunk2_size, file);
    if (read_size != wav->data_subchunk.subchunk2_size) {
        fprintf(stderr, "failed to read audio data from file\n");
        wav_file_free(wav);
    }

    return wav;
}

void wav_file_write(const wav_file_t* wav, FILE* file) {
    if (wav == NULL) {
        fprintf(stderr, "wav_file pointer is NULL\n");
        return;
    }

    if (file == NULL) {
        fprintf(stderr, "file pointer is NULL\n");
        return;
    }

    size_t write_size;

    write_size = fwrite(&wav->riff_header, sizeof(riff_header_t), 1, file);
    if (write_size != 1) {
        fprintf(stderr, "failed to write RIFF header to file\n");
        return;
    }

    write_size = fwrite(&wav->fmt_subchunk, sizeof(fmt_subchunk_t), 1, file);
    if (write_size != 1) {
        fprintf(stderr, "failed to write fmt subchunk to file\n");
        return;
    }

    write_size = fwrite(&wav->data_subchunk, sizeof(data_subchunk_t) - sizeof(((data_subchunk_t *)0)->data), 1, file);
    if (write_size != 1) {
        fprintf(stderr, "failed to write data subchunk to file\n");
        return;
    }

    write_size = fwrite(wav->data_subchunk.data, 1, wav->data_subchunk.subchunk2_size, file);
    if (write_size != wav->data_subchunk.subchunk2_size) {
        fprintf(stderr, "failed to write audio data to file\n");
        return;
    }
}
