/**
 * log.h: debug logger
 */
#ifndef LOG_H

#   define LOG_H

#   define  LogDbg      3
#   define  LogInfo     2
#   define  LogWarn     1
#   define  LogError    0

#   define log_d(text)          __log(LogDbg,text)
#   define log_i(text)          __log(LogInfo,text)
#   define log_w(text)          __log(LogWarn,text)
#   define log_e(text)          __log(LogError,text)
#   define __log(level,text)    dbglog(__FILE__,__LINE__,level,text)

#   define _eoom        "out of memory"
#   define _enull       "null"

#   ifdef __cplusplus
        extern "C" {
#   endif

    extern int dbglog(char *file,int line,int type,char *txt);
    extern void dbglog_set_handler(void (*_log_fn)(int type,char *text));

#   ifdef __cplusplus
        }
#   endif

#endif
