/**
 * Some os functions
 */
#include "os/os.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * tick data
 */
long _os_old_tick_handler = 0;
long _os_internal_ticks=0;
// our tick handler
void os_internal_tick_handler(void);

/**
 * @brief perform initialisation
 */
void os_initialise(void) {
    // install our tick handler
    _os_old_tick_handler=os_getvect(0x1c);
    os_setvect(0x1c,(long)os_internal_tick_handler);
}

/**
 * @brief shutdown
 */
void os_shutdown(void) {
    // reset the tick handler
    os_setvect(0x1c,(long)_os_old_tick_handler);
}

/**
 * @brief get the number of ticks since boot
 * @return ticks since boot
 */
long os_internal_ticks(void) {
    return _os_internal_ticks;
}

/**
 * @brief sleep some milliseconds
 * @param ms
 */
void os_sleep(uint32_t ms) {
    ms/=55;
    ms+=os_ticks();
    while(os_ticks()<ms);
}

/**
 * @brief allocat and zero some memory
 * @param file
 * @param line
 * @param size
 */
void *_os_malloc(char *file,int line,int size) {
    void *v=malloc(size);
    if(v) {
        memset(v,0,size);
    }
    else {
        char text[40];
        sprintf(text,"%s:%d oom %d bytes",file,line,size);
        log_e(text);
    }
    return v;
}

#ifndef linux  // hide assembly from linux compilers when warning/error checking

/*
 * assembler from here on
 */
#asm
.text

/**
 * @brief handle the 0x08 ticker
 *
 * void os_internal_tick_handler(void)
 */
export _os_internal_tick_handler
_os_internal_tick_handler:
    ! increment our tick count
    mov   ax,[__os_internal_ticks]
    inc   ax
    mov   [__os_internal_ticks],ax
    ! fake an interrupt call stack and call the old routine
    pushf
    callf [__os_old_tick_handler]
    iret

/**
 * @brief get the number of ticks since boot
 * @return ticks since boot
 *
 * long os_ticks(void)
 */
export _os_ticks
_os_ticks:
    push ds
    pushf
    cli

    mov  ax,#$0
    mov  ds,ax
    mov  bx,#$046C
    mov  ax,[bx+0]
    mov  dx,[bx+2]

    popf
    pop  ds
    ret

/**
 * @brief get an interrupt vector
 * @param vect
 * @return the current int vect
 *
 * long os_getvect(uint16_t vect)
 */
export _os_getvect
_os_getvect:
    push    bp
    mov     bp,sp
    push    es
    push    bx
    mov     ax,4[bp]
    mov     ah,#$35
    int     #$21
    mov     dx,es
    mov     ax,bx
    pop     bx
    pop     es
    pop     bp
    ret

/**
 * @brief set an interrupt vector
 * @param vect
 * @param routine
 *
 * void os_setvect(uint16_t vect,long routine)
 */
export _os_setvect
_os_setvect:
    push    bp
    mov     bp,sp
    push    ax
    push    bx
    push    dx

    mov     ax,4[bp]
    mov     dx,6[bp]
    mov     bx,8[bp]

    test    bx,bx
    jnz     ossv_seg
    mov     bx,cs       ! no seg, use cs

  ossv_seg:
    push    ds
    mov     ds,bx
    mov     ah,#$25
    int     #$21

    pop     ds
    pop     dx
    pop     bx
    pop     ax
    pop     bp
    ret

#endif // linux
