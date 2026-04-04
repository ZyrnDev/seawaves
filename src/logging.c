#include <raylib.h>
#include <stdio.h>

#include <sys/time.h>
#include <time.h>

#define LOG_ANSI_COLOUR_CLEAR "\033[0m"
#define LOG_ANSI_COLOUR_BLACK "\033[0;30m"
#define LOG_ANSI_COLOUR_RED "\033[0;31m"
#define LOG_ANSI_COLOUR_GREEN "\033[0;32m"
#define LOG_ANSI_COLOUR_YELLOW "\033[0;33m"
#define LOG_ANSI_COLOUR_BLUE "\033[0;34m"
#define LOG_ANSI_COLOUR_MAGENTA "\033[0;35m"
#define LOG_ANSI_COLOUR_CYAN "\033[0;36m"
#define LOG_ANSI_COLOUR_WHITE "\033[0;37m"
#define LOG_ANSI_COLOUR_BRIGHT_BLACK "\033[1;30m"
#define LOG_ANSI_COLOUR_BRIGHT_RED "\033[1;31m"
#define LOG_ANSI_COLOUR_BRIGHT_GREEN "\033[1;32m"
#define LOG_ANSI_COLOUR_BRIGHT_YELLOW "\033[1;33m"
#define LOG_ANSI_COLOUR_BRIGHT_BLUE "\033[1;34m"
#define LOG_ANSI_COLOUR_BRIGHT_MAGENTA "\033[1;35m"
#define LOG_ANSI_COLOUR_BRIGHT_CYAN "\033[1;36m"
#define LOG_ANSI_COLOUR_BRIGHT_WHITE "\033[1;37m"

// Custom logging funtion
void ColouredLog(int msgType, const char *text, va_list args) {

  struct timeval tv;
  gettimeofday(&tv, NULL);

  // Convert to local time
  struct tm *tm_info = localtime(&tv.tv_sec);

  printf(LOG_ANSI_COLOUR_BRIGHT_BLACK
         "%02d:%02d:%02d:%04d " LOG_ANSI_COLOUR_CLEAR,
         tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
         (int)(tv.tv_usec / 1000));

  switch (msgType) {
  case LOG_INFO:
    printf("[" LOG_ANSI_COLOUR_BLUE "INFO" LOG_ANSI_COLOUR_CLEAR "]: ");
    break;
  case LOG_ERROR:
    printf("[" LOG_ANSI_COLOUR_RED "ERROR" LOG_ANSI_COLOUR_CLEAR "]: ");
    break;
  case LOG_WARNING:
    printf("[" LOG_ANSI_COLOUR_YELLOW "WARNING" LOG_ANSI_COLOUR_CLEAR "]: ");
    break;
  case LOG_DEBUG:
    printf("[" LOG_ANSI_COLOUR_MAGENTA "DEBUG" LOG_ANSI_COLOUR_CLEAR "]: ");
    break;
  default:
    break;
  }

  vprintf(text, args);
  printf("\n");
}
