#ifndef UTILS_H
#define UTILS_H

#include <sys/printk.h>
#include <stdarg.h> // for variadic arguments

#if defined(ENABLE_DEBUG_LOG)

void custom_log(char *level, char *filename, int line, char *pre, char *format, ...);

#define DVC_TYPE "C "

#define LOG_DEBUG(format, ...) \
    custom_log("DEBUG", __FILE__, __LINE__, DVC_TYPE "[%s] [%s:%d] ", format "\n", ##__VA_ARGS__);
#define LOG_TRACE(format, ...) \
    custom_log("TRACE", __FILE__, __LINE__, DVC_TYPE "[%s] [%s:%d] ", format "\n", ##__VA_ARGS__);
#define LOG_INFO(format, ...) \
    custom_log("INFO", __FILE__, __LINE__, DVC_TYPE "[%s] [%s:%d] ", format "\n", ##__VA_ARGS__);
#define LOG_ERROR(format, ...) \
    custom_log("ERROR", __FILE__, __LINE__, DVC_TYPE "[%s] [%s:%d] ", format "\n", ##__VA_ARGS__);

#else // defined(ENABLE_DEBUG_LOG)

#define LOG_DEBUG(format, ...)
#define LOG_TRACE(format, ...)
#define LOG_INFO(format, ...) printk(format "\n", ##__VA_ARGS__);
#define LOG_ERROR(format, ...) printk("[ERROR] " format "\n", ##__VA_ARGS__);

#endif // defined(ENABLE_DEBUG_LOG)

#endif // UTILS_H
