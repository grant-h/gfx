#include "Log.hpp"

#include <stdio.h>
#include <stdarg.h>

enum LogLevel log_level = DEFAULT_LOG_LEVEL;

void log_set_level(enum LogLevel level)
{
  log_level = level;
}

void _log_add_line(enum LogLevel level, const char * fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  switch(level)
  {
    case LOG_LEVEL_FATAL:
      fprintf(stderr, "[FATAL] ");
      break;
    case LOG_LEVEL_ERROR:
      fprintf(stderr, "[ERROR] ");
      break;
    case LOG_LEVEL_WARN:
      fprintf(stderr, "[WARN] ");
      break;
    case LOG_LEVEL_DEBUG:
      fprintf(stderr, "[DEBUG] ");
      break;
    case LOG_LEVEL_INFO:
      fprintf(stderr, "[INFO] ");
      break;
    case LOG_LEVEL_TRACE:
      fprintf(stderr, "[TRACE] ");
      break;
    default:
      break;
  }

  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");

  va_end(args);
}
