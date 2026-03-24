#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "log.h"

void copy_str(const char* src, char** dest) {
    if (src == NULL || dest == NULL) {
        return;
    }

    size_t len = strlen(src) + 1;
    *dest = malloc(len);
    if (*dest == NULL) {
        perror("Failed to allocate memory for string copy");
        exit(EXIT_FAILURE);
    }

    strncpy(*dest, src, len);
}

configuration_t configuration_create(void) {
    configuration_t config;
    config.action = NULL;
    config.source_path = NULL;
    config.destination_path = NULL; // Default value
    return config;
}

void configuration_free(configuration_t* config) {
    if (config == NULL) {
        return;
    }

    if (config->action != NULL) {
        free(config->action);
        config->action = NULL;
    }

    if (config->source_path != NULL) {
        free(config->source_path);
        config->source_path = NULL;
    }

    if (config->destination_path != NULL) {
        free(config->destination_path);
        config->destination_path = NULL;
    }
}

void configuration_parse_args(configuration_t* config, int argc, char* argv[]) {
    if (config == NULL) {
        LOG_FATAL("configuration pointer is NULL");
    }

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            fprintf(stderr, "Usage: %s [OPTIONS] {copy | generate}>\n", argv[0]);
            fprintf(stderr, "OPTIONS:\n");
            fprintf(stderr, " -s <path>, --source <path>       Source file path\n");
            fprintf(stderr, " -d <path>, --destination <path>  Destination file path\n");
            exit(EXIT_SUCCESS);
        } else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--destination") == 0)) {
            if (i + 1 < argc) {
                copy_str(argv[i + 1], &config->destination_path);
                i++; // Skip next argument as it's the value
            } else {
                LOG_FATAL("missing value for %s", argv[i]);
            }
        } else if ((strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--source") == 0)) {
            if (i + 1 < argc) {
                copy_str(argv[i + 1], &config->source_path);
                i++; // Skip next argument as it's the value
            } else {
                LOG_FATAL("missing value for %s", argv[i]);
            }
        } else {
            // Assume the first non-option argument is the action
            if (config->action == NULL) {
                copy_str(argv[i], &config->action);
            } else {
                LOG_FATAL("unexpected argument: %s", argv[i]);
            }
        }
    }

    if (config->action == NULL) {
        fprintf(stderr, "Usage: %s [OPTIONS] {copy | generate}\n", argv[0]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

void configuration_print(const configuration_t* config) {
    if (config == NULL) {
        fprintf(stderr, "configuration pointer is NULL\n");
        return;
    }

    printf("configuration:\n");
    printf("  action: %s\n", config->action ? config->action : "NULL");
    printf("  source_path: %s\n", config->source_path ? config->source_path : "NULL");
    printf("  destination_path: %s\n", config->destination_path ? config->destination_path : "NULL");
}
