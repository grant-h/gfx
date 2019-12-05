#ifndef _LOG_H
#define _LOG_H

// The higher the log level, the more output
enum LogLevel
{
    LOG_LEVEL_DISABLED = 0,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
};

#define DEFAULT_LOG_LEVEL LOG_LEVEL_INFO

void log_increase_level();
void log_set_level(enum LogLevel level);

extern enum LogLevel log_level;

inline bool _log_should_log(enum LogLevel level)
{
  return level <= log_level;
}

// these have to be public, but should never be called directly
void _log_add_line(enum LogLevel level, const char * fmt, ...);

// Logging macros. Meant to be used
#define LOG_BODY(level, fmt, ...)               \
    do {                                        \
        if(_log_should_log(level))                    \
            _log_add_line(level, fmt, ##__VA_ARGS__); \
    } while(0)

#define LOG_FATAL(fmt, ...) \
    LOG_BODY(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    LOG_BODY(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    LOG_BODY(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    LOG_BODY(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
    LOG_BODY(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#define LOG_TRACE(fmt, ...) \
    LOG_BODY(LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)

#define LOG_TRACE_FUNC(fmt, ...) \
    LOG_BODY(LOG_LEVEL_TRACE, "%s:%d %s() " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif // _LOG_H
