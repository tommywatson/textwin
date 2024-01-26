/**
 * log.c: debug logger
 */
#include "os/log.h"
#include "os/vga.h"

#include <string.h>
#include <stdio.h>

#define LOGTODISKx // false

#define LOG_MAX     80

uint16_t log_type = LogDbg;
uint32_t log_ticks = 0;
char log_text[LOG_MAX];
uint16_t log_background[LOG_MAX];
uint16_t log_background_len;

void (*_log_fn)(int type,char *text);

#ifdef LOGTODISK
#include <stdio.h>
FILE *fp;
#endif

/**
 * @brief
 */
void dbglog_print(void) {
    if(log_ticks) {
        if(log_ticks<os_internal_ticks()) {
            // Clean up the old log message
            if(log_background_len) {
                vga_paste(log_background,
                          40-log_background_len/2,
                          0,
                          log_background_len);
            }
            // reset back to debug
            log_type=LogDbg;
            log_ticks=0;
            log_background_len=0;
        }
        else {
            uint8_t colour=0;
            uint16_t x=LOG_MAX/2-strlen(log_text)/2;
            switch(log_type) {
                case LogDbg: {
                    colour=WHITE|BLACK<<4;
                    break;
                }
                case LogInfo:{
                    colour=WHITE|GREEN<<4;
                    break;
                }
                case LogWarn:{
                    colour=WHITE|BLUE<<4;
                    break;
                }
                case LogError:{
                    colour=WHITE|RED<<4;
                    break;
                }
            }
            vga_print(x,0,colour,log_text);
        }
    }
}

/**
 * @brief dbglog
 * @param file
 * @param line
 * @param type
 * @param txt
 * @return true (so it can be used in macro tests)
 */
int dbglog(char *file,int line,int type,char *txt) {

#ifdef LOGTODISK
    if(!fp) fp=fopen("log.txt","wt");
    fprintf(fp,"%05lu %s:%d %d %s\n",os_internal_ticks(),file,line,type,txt);
    fflush(0);
#endif

    if(_log_fn) {
        *log_text=0;
        sprintf(log_text,"%s:%d %s",file,line,txt);
        *(log_text+sizeof(log_text)-1)=0;
        _log_fn(type,log_text);
    }
    else {
        if(type<=log_type) {
            if(log_background_len) {
                vga_paste(log_background,
                          40-log_background_len/2,
                          0,
                          log_background_len);
            }
            log_type=type;
            sprintf(log_text,"%s:%d %s",file,line,txt);
            *(log_text+sizeof(log_text)-1)=0;
            log_background_len=strlen(log_text);
            vga_copy(log_background,
                     40-log_background_len/2,
                     0,
                     log_background_len);
            log_ticks=os_internal_ticks()+5*5-type*5;
            dbglog_print();
        }
    }
    return 1;
}

/**
 * @brief set the log handler
 */
void dbglog_set_handler(void (*handler)(int type,char *text)) {
    _log_fn=handler;
}
