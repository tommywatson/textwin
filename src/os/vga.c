/**
 * vga.h
 *
 * Basic VGA functions
 */
#include "os/vga.h"

#ifndef linux  // hide assembly from linux compilers when warning/error checking

#asm
.text

/**
 * @brief set 80x25 text mode
 *
 * void vga_80x25(void)
 */
export _vga_80x25
_vga_80x25:
    mov   ax,#$03;
    int   #$10
    ret

/**
 * @brief set 300x200 256 colours
 *
 * void vga_320x200(void)
 */
export _vga_320x200
_vga_320x200:
    mov   ax,#$13;
    int   #$10
    ret

/**
 * @brief Print some text at [x,y]
 * @param x
 * @param y
 * @param colour
 * @param text
 *
 * void vga_print(uint16_t x,uint16_t y,uint16_t colour,char *text)
 */
export _vga_print
_vga_print:
    push    bp
    mov     bp,sp
    push    ds
    push    es
    push    si
    push    di
    push    ax

    ! es:di destination
    mov     ax,#$b800
    mov     es,ax
    mov     di,6[bp]    ! y
    imul    di,#80      ! y * 80
    add     di,4[bp]    ! y * 80 + x
    imul    di,#2       ! (y * 80 + x) * 2

    ! ds:si source
    mov     si,10[bp]   ! text

    ! colour
    mov     ah,8[bp]    ! colour

    cld                 ! forward direction
print_next:
    lodsb               ! ds:si -> al
    cmp     al,0        ! check *text for end of string
    jz      print_done
    stosw               ! ax -> es:di
    jmp     print_next
print_done:

    pop     ax
    pop     di
    pop     si
    pop     es
    pop     ds
    pop     bp
    ret

/**
 * @brief Clear the screen
 * @param colour
 *
 * void vga_clear(uint16_t colour)
 */
export _vga_clear
_vga_clear:
    push    bp
    mov     bp,sp
    push    es
    push    di
    push    ax
    push    cx

    ! es:di destination
    mov     ax,#$b800
    mov     es,ax
    xor     di,di

    ! colour
    xor     ax,ax
    mov     ah,4[bp]    ! colour
    shl     ah,4        ! set background

    mov     cx,#2000    ! 80x25 = 2000 characters
    cld                 ! forward direction
    rep
    stosw               ! ax -> es:di

    pop     cx
    pop     ax
    pop     di
    pop     es
    pop     bp
    ret

/**
 * @brief Copy some data
 * @param dest desination pointer
 * @param x
 * @param y
 * @param len
 *
 * void vga_copy(uint16_t *dest,uint16_t x,uint16_t y,uint16_t len)
 */
export _vga_copy
_vga_copy:
    push    bp
    mov     bp,sp
    push    ds
    push    es
    push    si
    push    di
    push    cx

    mov     di,ds
    mov     es,di

    mov     di,#$b800
    mov     ds,di
    mov     di,4[bp]
    mov     si,8[bp]
    imul    si,#80
    add     si,6[bp]
    imul    si,#2

    mov     cx,10[bp]
    cld
    rep
    movsw   ! ds:si -> es:di

    pop     cx
    pop     di
    pop     si
    pop     es
    pop     ds
    pop     bp
    ret

/**
 * @brief Paste some data
 * @param dest desination pointer
 * @param x
 * @param y
 * @param len
 *
 * void vga_paste(uint16_t *src,uint16_t x,uint16_t y,uint16_t len)
 */
export _vga_paste
_vga_paste:
    push    bp
    mov     bp,sp
    push    es
    push    si
    push    di
    push    cx

    mov     di,#$b800
    mov     es,di

    mov     si,4[bp]
    mov     di,8[bp]
    imul    di,#80
    add     di,6[bp]
    imul    di,#2

    mov     cx,10[bp]
    cld
    rep
    movsw   ! ds:si -> es:di

    pop     cx
    pop     di
    pop     si
    pop     es
    pop     bp
    ret

/**
 * @brief blast the 80x25 data into the screen buffer
 * @param src
 *
 * void vga_blit_80x25(uint16_t *src);
 */
export _vga_blit_80x25
_vga_blit_80x25:
    push    bp
    mov     bp,sp
    push    es
    push    si
    push    di
    push    cx

    mov     di,#$b800
    mov     es,di

    mov     si,4[bp]
    xor     di,di

    mov     cx,#2000    ! 80x25
    cld
    rep
    movsw   ! ds:si -> es:di

    pop     cx
    pop     di
    pop     si
    pop     es
    pop     bp
    ret

/**
 * @brief hide the cursor
 *
 * void cursor_hide(void);
 */
export _cursor_hide
_cursor_hide:
    push    cx
    xor     ax,ax
    xor     cx,cx
    mov     ah,#1
    mov     ch,#$20
    int     #$10
    pop     cx
    ret

/**
 * @brief show the cursor
 *
 * void cursor_show(void);
 */
export _cursor_show
_cursor_show:
    push    cx
    xor     ax,ax
    xor     cx,cx
    mov     ah,#1
    mov     cl,#$4  ! not sure what this really does
    int     #$10
    pop     cx
    ret


#endif // linux
