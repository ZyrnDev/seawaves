#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"
#include "wave.h"

#define PI 3.141592653589793238463

double random_sample(void) {
  return (double)rand() / (double)RAND_MAX * 2.0 -
         1.0; // Random value between -1 and 1
}

void save_samples(uint8_t *buffer, size_t index, uint8_t num_samples,
                  double *samples, uint8_t bits_per_sample,
                  uint8_t block_align) {
  if (buffer == NULL || samples == NULL)
    LOG_FATAL("buffer or samples pointer is NULL");
  if (block_align > 8)
    LOG_FATAL("block_align must be less than or equal to 8");

  uint64_t value = 0;

  for (size_t c = 0; c < num_samples; c++) {
    if (samples[c] < -1.0 || samples[c] > 1.0) {
      LOG_WARNING("sample %zu is out of range: %.6f (clamping to [-1, 1])", c,
                  samples[c]);
      if (samples[c] < -1.0)
        samples[c] = -1.0;
      if (samples[c] > 1.0)
        samples[c] = 1.0;
    }
  }

  for (size_t c = 0; c < num_samples; c++) {
    double sample = samples[c];
    sample = (sample + 1.0) / 2.0;        // Normalize to [0, 1]
    sample *= (1 << bits_per_sample) - 1; // Scale to [0, max_value(bit_depth)]

    uint64_t sample_value = (uint64_t)sample;
    // LOG_DEBUG("sample %.6f -> %zu", sample, sample_value);
    value <<= bits_per_sample;
    value |= sample_value & ((1 << bits_per_sample) - 1);
  }

  // left align the value to the container size
  // value <<= 64 - (num_samples * bits_per_sample);

  uint8_t *data = (uint8_t *)&value;
  for (size_t b = 0; b < block_align; b++) {
    buffer[index * block_align + b] = data[b];
  }
}

// int main(int argc, char *argv[]) {
//     log_set_stream(stderr);
//     // log_printf(LOG_LEVEL_INFO, "Seawaves - A simple WAV file
//     reader/writer");
//     // TEST("Seawaves - A simple WAV file reader/writer");    //
//     LOG_INFO("Seawaves - A simple WAV file reader/writer");    //
//     configuration_t config = configuration_create();

//     configuration_parse_args(&config, argc, argv);
//     configuration_print(&config);

//     // FILE* dest_file = fopen("test.wav", "rb");
//     // if (dest_file == NULL) {
//     //     LOG_FATAL("unable to open destination file: %s", strerror(errno));
//     //     return 1;
//     // }

//     // wav_file_t* wav_file = wav_file_read(dest_file);
//     // if (wav_file == NULL) {
//     //     fprintf(stderr, "Error reading WAV file\n");
//     //     fclose(dest_file);
//     //     configuration_free(&config);
//     //     exit(EXIT_FAILURE);
//     // }

//     // wav_file_print(wav_file);

//     // wav_file_free(wav_file);

//     // fclose(dest_file);

//     if (strcmp(config.action, "generate") == 0) {
//         LOG_INFO("Generating WAV file: %s", config.destination_path);

//         size_t duration = 20; // seconds
//         uint32_t sample_rate = 44100; // Hz
//         uint8_t bit_depth = 16; // bits
//         uint16_t block_align = 4; // bytes per block (all channels)
//         uint16_t num_channels = 2; // Stereo

//         wav_file_t* wav_file = wav_file_create();

//         *(uint32_t*) wav_file->riff_header.chunk_id = 0x46464952; // "RIFF"
//         *(uint32_t*) wav_file->riff_header.format = 0x45564157;   // "WAVE"
//         *(uint32_t*) wav_file->fmt_subchunk.subchunk1_id = 0x20746d66; //
//         "fmt " wav_file->fmt_subchunk.subchunk1_size = 16; // PCM
//         wav_file->fmt_subchunk.audio_format = 1; // PCM
//         wav_file->fmt_subchunk.num_channels = num_channels; // Stereo
//         wav_file->fmt_subchunk.sample_rate = sample_rate; // 44100 Hz
//         wav_file->fmt_subchunk.byte_rate = sample_rate * block_align; //
//         SampleRate * NumChannels * BytesPerSample
//         wav_file->fmt_subchunk.block_align = block_align; // NumChannels *
//         BitsPerSample/8 wav_file->fmt_subchunk.bits_per_sample = bit_depth;
//         // 1s
//         *(uint32_t*) wav_file->data_subchunk.subchunk2_id = 0x61746164; //
//         "data"

//         wav_file->data_subchunk.subchunk2_size =
//         wav_file->fmt_subchunk.byte_rate * duration;
//         wav_file->riff_header.chunk_size = 36 +
//         wav_file->data_subchunk.subchunk2_size;
//         wav_file->riff_header.chunk_size = 4 + (8 +
//         wav_file->fmt_subchunk.subchunk1_size) + (8 +
//         wav_file->data_subchunk.subchunk2_size); wav_file->data_subchunk.data
//         = malloc(wav_file->data_subchunk.subchunk2_size); if
//         (wav_file->data_subchunk.data == NULL) {
//             LOG_FATAL("unable to allocate memory for WAV data");
//         }

//         double* samples = malloc(wav_file->fmt_subchunk.num_channels *
//         sizeof(double)); if (samples == NULL) {
//             LOG_FATAL("unable to allocate memory for samples");
//         }

//         size_t num_samples = wav_file->data_subchunk.subchunk2_size /
//         wav_file->fmt_subchunk.block_align; for (size_t i = 0; i <
//         num_samples; i++) {
//             // double period = wav_file->fmt_subchunk.sample_rate * 5.0; // 2
//             seconds
//             // double t = (double)i;
//             // double volume = fabs(sin(t/period * PI)); // Volume envelope
//             (fade in/out)
//             // double r = random_sample() * volume;

//             double frequency;
//             switch (i / wav_file->fmt_subchunk.sample_rate) {
//                 case 0:
//                     frequency = 440.0f; // A4
//                     break;
//                 case 1:
//                     frequency = 554.37f; // C#5
//                     break;
//                 case 2:
//                     frequency = 659.25f; // E5
//                     break;
//                 case 3:
//                     frequency = 880.0f; // A5
//                     break;
//                 default:
//                     frequency = 440.0f; // A4
//                     break;
//             }
//             double amplitude = 0.0125f;
//             double sample = amplitude * sinf(2.0f * PI * frequency * i /
//             wav_file->fmt_subchunk.sample_rate);

//             for (size_t c = 0; c < wav_file->fmt_subchunk.num_channels; c++)
//             {
//                 samples[c] = sample;
//             }

//             save_samples(wav_file->data_subchunk.data, i,
//             wav_file->fmt_subchunk.num_channels, samples,
//             wav_file->fmt_subchunk.bits_per_sample,
//             wav_file->fmt_subchunk.block_align);
//         }

//         free(samples);

//         LOG_INFO("WAV file generated");

//         // wav_file_print(wav_file);

//         FILE* test_file = fopen(config.destination_path, "wb");
//         if (test_file == NULL) {
//             LOG_FATAL("unable to open test file for writing: %s",
//             strerror(errno));
//         }

//         wav_file_write(wav_file, test_file);

//         wav_file_free(wav_file);
//     }

// //
// ///////////////////////////////////////////////////////////////////////////////

// //     FILE* dest_file = fopen(config.destination_path, "rb");
// //     if (dest_file == NULL) {
// //         LOG_FATAL("unable to open destination file: %s", strerror(errno));
// //         return 1;
// //     }

// //     wav_file_t* wav_file = wav_file_read(dest_file);
// //     if (wav_file == NULL) {
// //         fprintf(stderr, "Error reading WAV file\n");
// //         fclose(dest_file);
// //         configuration_free(&config);
// //         exit(EXIT_FAILURE);
// //     }

// //     fclose(dest_file);

// //
// ///////////////////////////////////////////////////////////////////////////////

// //     wav_file_print(wav_file);

// //     float max_amplitude = powf(2, wav_file->fmt_subchunk.bits_per_sample -
// 1) - 1;

// //     int sample_bytes = wav_file->fmt_subchunk.bits_per_sample / 8;
// //     for (size_t i = 0; i < 10 && i <
// wav_file->data_subchunk.subchunk2_size / sample_bytes; i++) {
// //         size_t value = 0;
// //         for (int b = 0; b < sample_bytes; b++) {
// //             value |= wav_file->data_subchunk.data[i * sample_bytes + b] <<
// (b * 8);
// //         }

// //         float sample = value / max_amplitude - 1;
// //         printf("Sample %zu: 0x%0*zx | %0*zu | %.6f\n", i, sample_bytes *
// 2, value, sample_bytes * 3, value, sample);
// //     }

// //
// ///////////////////////////////////////////////////////////////////////////////

// //     FILE* test_file = fopen("test2.wav", "wb");
// //     if (test_file == NULL) {
// //         perror("Error opening test file for writing");
// //         wav_file_free(wav_file);
// //         exit(EXIT_FAILURE);
// //     }

// //     wav_file_write(wav_file, test_file);

// //     fclose(test_file);

// //
// ///////////////////////////////////////////////////////////////////////////////

// //     wav_file_free(wav_file);

//     configuration_free(&config);

//     return 0;
// }
