#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// By default, log everything
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

typedef enum {
    LOG_LEVEL_FATAL = 0x00,
    LOG_LEVEL_ERROR = 0x10,
    LOG_LEVEL_WARNING = 0x20,
    LOG_LEVEL_INFO = 0x30,
    LOG_LEVEL_DEBUG = 0x40,
} LogLevel;

#define LOG_ANSI_COLOUR_CLEAR        "\033[0m"
#define LOG_ANSI_COLOUR_BLACK        "\033[0;30m"
#define LOG_ANSI_COLOUR_RED          "\033[0;31m"
#define LOG_ANSI_COLOUR_GREEN        "\033[0;32m"
#define LOG_ANSI_COLOUR_YELLOW       "\033[0;33m"
#define LOG_ANSI_COLOUR_BLUE         "\033[0;34m"
#define LOG_ANSI_COLOUR_MAGENTA      "\033[0;35m"
#define LOG_ANSI_COLOUR_CYAN         "\033[0;36m"
#define LOG_ANSI_COLOUR_WHITE        "\033[0;37m"
#define LOG_ANSI_COLOUR_BRIGHT_BLACK "\033[1;30m"
#define LOG_ANSI_COLOUR_BRIGHT_RED   "\033[1;31m"
#define LOG_ANSI_COLOUR_BRIGHT_GREEN "\033[1;32m"
#define LOG_ANSI_COLOUR_BRIGHT_YELLOW "\033[1;33m"
#define LOG_ANSI_COLOUR_BRIGHT_BLUE  "\033[1;34m"
#define LOG_ANSI_COLOUR_BRIGHT_MAGENTA "\033[1;35m"
#define LOG_ANSI_COLOUR_BRIGHT_CYAN  "\033[1;36m"
#define LOG_ANSI_COLOUR_BRIGHT_WHITE "\033[1;37m"

void log_set_stream(FILE* stream);
FILE* log_get_stream(void);
static inline void log_printf(LogLevel level, const char *fmt, ...);

///////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////
static inline void log_printf(LogLevel level, const char *fmt, ...) {
    FILE* stream = log_get_stream();
    if ((level > LOG_LEVEL && level != LOG_LEVEL_FATAL) || stream == NULL) {
        return;
    }

    // Fetch current time
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Convert to local time
    struct tm *tm_info = localtime(&tv.tv_sec);

    // Print formatted time
    fprintf(stream,
        LOG_ANSI_COLOUR_BRIGHT_BLACK
            "%02d:%02d:%02d:%04d "
        LOG_ANSI_COLOUR_CLEAR,
        tm_info->tm_hour,
        tm_info->tm_min,
        tm_info->tm_sec,
        (int)(tv.tv_usec / 1000)
    );

    // Print timestamp

    va_list args;
    va_start(args, fmt);
    vfprintf(stream, fmt, args);
    va_end(args);

    fprintf(stream, "\n");
}

#define LOG_FATAL(fmt, ...) { \
    log_printf(LOG_LEVEL_FATAL, \
        LOG_ANSI_COLOUR_RED \
            "[fatal] " \
        LOG_ANSI_COLOUR_CLEAR \
            fmt, ##__VA_ARGS__); \
    if (log_get_stream() != NULL) { \
        fclose(log_get_stream()); \
    } \
    exit(EXIT_FAILURE); \
}

#define LOG_ERROR(fmt, ...) log_printf(\
    LOG_LEVEL_ERROR, \
    LOG_ANSI_COLOUR_RED "[error] " LOG_ANSI_COLOUR_CLEAR \
    fmt, \
    ##__VA_ARGS__\
)

#define LOG_WARNING(fmt, ...) log_printf(\
    LOG_LEVEL_WARNING, \
    LOG_ANSI_COLOUR_YELLOW "[warning] " LOG_ANSI_COLOUR_CLEAR \
    fmt, \
    ##__VA_ARGS__\
)

#define LOG_INFO(fmt, ...) log_printf(\
    LOG_LEVEL_INFO, \
    LOG_ANSI_COLOUR_CYAN "[info] " LOG_ANSI_COLOUR_CLEAR \
    fmt, \
    ##__VA_ARGS__\
)

#define LOG_DEBUG(fmt, ...) log_printf(\
    LOG_LEVEL_DEBUG, \
    LOG_ANSI_COLOUR_MAGENTA "[debug] " LOG_ANSI_COLOUR_CLEAR \
    fmt, \
    ##__VA_ARGS__\
)

#endif // LOG_H
