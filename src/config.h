#ifndef __SEAWAVES_CONFIG_H__
#define __SEAWAVES_CONFIG_H__

typedef struct configuration {
    char* action;
    char* source_path;
    char* destination_path;
} configuration_t;

configuration_t configuration_create(void);
void configuration_free(configuration_t* config);
void configuration_parse_args(configuration_t* config, int argc, char* argv[]);
void configuration_print(const configuration_t* config);

#endif // __SEAWAVES_CONFIG_H__
