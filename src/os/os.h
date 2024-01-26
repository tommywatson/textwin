/**
 * Some basic os functions
 */
#ifndef OS_H

#   define OS_H

#   ifndef linux
    typedef char int8_t;
    typedef unsigned char uint8_t;
    typedef short int16_t;
    typedef unsigned short uint16_t;
    typedef long int32_t;
    typedef unsigned long uint32_t;
#   else
#       include <stdint.h>
#   endif   // linux

#   include "os/log.h"

#   ifdef __cplusplus
        extern "C" {
#   endif

    extern void os_initialise(void);
    extern void os_shutdown(void);
    extern void os_sleep(uint32_t ms);
    extern long os_ticks(void);
    extern long os_internal_ticks(void);

#   define os_malloc(size) _os_malloc(__FILE__,__LINE__,size);
    extern void *_os_malloc(char *file,int line,int size);

    extern long os_getvect(uint16_t vect);
    extern void os_setvect(uint16_t vect,long routine);

#   ifdef __cplusplus
        }
#   endif

#endif // OS_H
