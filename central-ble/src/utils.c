#include "utils.h"

#if defined(ENABLE_DEBUG_LOG)
void custom_log(char *level, char *filename, int line, char *pre, char *format, ...)
{
    va_list vargs;

    printk(pre, level, filename, line);

    va_start(vargs, format);
    vprintk(format, vargs);
    va_end(vargs);
} // logger_log
#endif // defined(ENABLE_DEBUG_LOG)
