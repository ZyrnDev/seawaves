#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

configuration_t configuration_create() {
    configuration_t config;
    config.destination_path = NULL; // Default value
    return config;
}

void configuration_free(configuration_t* config) {
    if (config == NULL) {
        return;
    }

    if (config->destination_path != NULL) {
        free(config->destination_path);
        config->destination_path = NULL;
    }
}

void configuration_parse_args(configuration_t* config, int argc, char* argv[]) {
    if (config == NULL) {
        fprintf(stderr, "configuration pointer is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // copy the destination path from command line argument
    size_t len = strlen(argv[1]) + 1;
    config->destination_path = malloc(len);
    if (config->destination_path == NULL) {
        perror("Failed to allocate memory for destination path");
        exit(EXIT_FAILURE);
    }
    strncpy(config->destination_path, argv[1], len);
}

void configuration_print(const configuration_t* config) {
    if (config == NULL) {
        fprintf(stderr, "configuration pointer is NULL\n");
        return;
    }

    printf("configuration:\n");
    printf("  destination_path: %s\n", config->destination_path ? config->destination_path : "NULL");
}
