#include "log.h"

static FILE* log_stream = NULL;

void log_set_stream(FILE* stream) {
    log_stream = stream;
}

FILE* log_get_stream(void) {
    return log_stream;
}
