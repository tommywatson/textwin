/*
 * mouse functions
 */
#include "os/mouse.h"

#ifndef linux  // hide assembly from linux compilers when warning/error checking

#asm
    .data
        ! ring buffer destination pointer for mouse events
        _mouse_dst:
        .word 0x00
        ! ring buffer source pointer for mouse events
        _mouse_src:
        .word 0x00
        ! mouse data ring buffer, can hold 8 events
        _mouse_data:

        .word 0x00,0x00,0x00,0x00,0x00,0x00
        .word 0x00,0x00,0x00,0x00,0x00,0x00
        .word 0x00,0x00,0x00,0x00,0x00,0x00
        .word 0x00,0x00,0x00,0x00,0x00,0x00
        .word 0x00,0x00,0x00,0x00,0x00,0x00
        .word 0x00,0x00,0x00,0x00,0x00,0x00
        .word 0x00,0x00,0x00,0x00,0x00,0x00
        .word 0x00,0x00,0x00,0x00,0x00,0x00

    .text


/**
 * @brief hide the mouse cursore
 *
 * void mouse_hide(void)
 */
export _mouse_hide
_mouse_hide:
    mov ax,#$2
    int #$33
    ret

/**
 * @brief show the mouse cursor
 *
 * void mouse_show(void)
 */
export _mouse_show
_mouse_show:
    mov ax,#$1
    int #$33
    ret

/**
 * @brief handle mouse events
 *
 * void mouse_handler(void)
 */
export _mouse_handler
_mouse_handler:
    push    ds
    push    ax
    push    di
    push    ax

    mov     ax,cs
    add     ax,#__segoff
    mov     ds,ax
    mov     ax,[_mouse_dst]     ! dst
    imul    ax,#12
    mov     di,#_mouse_data
    add     di,ax
    pop     ax                  ! ax
    mov     0[di],ax
    mov     2[di],bx
    mov     4[di],cx
    mov     6[di],dx
    mov     8[di],si
    pop     ax                  ! di
    mov     10[di],ax
    mov     di,ax               ! reset di
    mov     ax,[_mouse_dst]     ! dst
    inc     ax
    and     ax,#$7
    cmp     ax,[_mouse_src]
    je      mh_fowf
    mov     [_mouse_dst],ax
    pop     ax
    pop     ds
    retf

  mh_fowf:
    call _dbg_do_something
    pop     ax
    pop     ds
    retf

/**
 * @brief install the mouse handler
 *
 * void mouse_handler_install(long handler)
 */
export _mouse_handler_install
_mouse_handler_install:
    push    bp
    mov     bp,sp
    push    ax
    push    cx
    push    dx
    push    es

    mov     dx,4[bp]    ! offs
    mov     ax,6[bp]    ! seg (maybe)
    test    ax,ax
    jnz     handler_seg_valid
    mov     ax,cs
  handler_seg_valid:
    mov     es,ax

    mov     ax,#$0c
    mov     cx,#$7f
    int     #$33

    pop     es
    pop     dx
    pop     cx
    pop     ax
    pop     bp
    ret

/**
 * @brief remove the mouse handler
 *
 * void mouse_handler_remove(void)
 */
export _mouse_handler_remove
_mouse_handler_remove:
    push    ax
    push    cx
    push    dx
    push    es

    mov     ax,#$0c
    mov     cx,#$00
    xor     dx,dx
    mov     es,dx
    int     #$33

    pop     es
    pop     dx
    pop     cx
    pop     ax
    ret

/**
 * @brief check for mouse data
 * @return true if available
 *
 * int mouse_data_available(void)
 */
export _mouse_data_available;
_mouse_data_available:
    mov     ax,[_mouse_dst]
    cmp     ax,[_mouse_src]
    je      no_mouse_data
    mov     ax,#1
    ret
  no_mouse_data:
    xor     ax,ax
    ret

/**
 * @brief Get the mouse data if available
 * @param data
 * @return true if available
 *
 * int mouse_get_data(MouseData *data)
 */
export _mouse_get_data
_mouse_get_data:
    mov     ax,[_mouse_src]
    cmp     ax,[_mouse_dst]
    je      mgd_no_mouse_data

    push    bp
    mov     bp,sp
    push    es
    push    di
    push    si
    push    cx

    imul    ax,#12
    mov     si,#_mouse_data
    add     si,ax

    mov     ax,ds
    mov     es,ax
    mov     di,4[bp]

    mov     cx,#6
    cld
    rep
    movsw   ! ds:si -> es:di

    mov     ax,[_mouse_src]
    inc     ax
    and     ax,#$7
    cli
    mov     [_mouse_src],ax
    sti

    pop     cx
    pop     si
    pop     di
    pop     es
    pop     bp
    mov     ax,#1
    ret
  mgd_no_mouse_data:
    xor     ax,ax
    ret

#endif // linux
