#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "wave.h"

int main(int argc, char *argv[]) {
    configuration_t config = configuration_create();

    configuration_parse_args(&config, argc, argv);
    configuration_print(&config);

///////////////////////////////////////////////////////////////////////////////

    FILE* dest_file = fopen(config.destination_path, "rb");
    if (dest_file == NULL) {
        perror("Error opening destination file");
        return 1;
    }

    wav_file_t* wav_file = wav_file_read(dest_file);
    if (wav_file == NULL) {
        fprintf(stderr, "Error reading WAV file\n");
        fclose(dest_file);
        configuration_free(&config);
        exit(EXIT_FAILURE);
    }

    fclose(dest_file);

///////////////////////////////////////////////////////////////////////////////

    wav_file_print(wav_file);

    float max_amplitude = powf(2, wav_file->fmt_subchunk.bits_per_sample - 1) - 1;

    int sample_bytes = wav_file->fmt_subchunk.bits_per_sample / 8;
    for (size_t i = 0; i < 10 && i < wav_file->data_subchunk.subchunk2_size / sample_bytes; i++) {
        size_t value = 0;
        for (int b = 0; b < sample_bytes; b++) {
            value |= wav_file->data_subchunk.data[i * sample_bytes + b] << (b * 8);
        }

        float sample = value / max_amplitude - 1;
        printf("Sample %zu: 0x%0*zx | %0*zu | %.6f\n", i, sample_bytes * 2, value, sample_bytes * 3, value, sample);
    }

///////////////////////////////////////////////////////////////////////////////

    FILE* test_file = fopen("test2.wav", "wb");
    if (test_file == NULL) {
        perror("Error opening test file for writing");
        wav_file_free(wav_file);
        exit(EXIT_FAILURE);
    }

    wav_file_write(wav_file, test_file);

    fclose(test_file);

///////////////////////////////////////////////////////////////////////////////

    wav_file_free(wav_file);

    configuration_free(&config);

    return 0;
}
