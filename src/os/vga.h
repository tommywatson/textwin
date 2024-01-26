/**
 * vga.h
 *
 * Basic VGA functions
 */
#ifndef VGA_H

#   define VGA_H

#   include "os/os.h"
#   include "os/vgacolours.h"

#   ifdef __cplusplus
        extern "C" {
#   endif

    extern void vga_80x25(void);
    extern void vga_320x200(void);

    extern void vga_print(uint16_t x,uint16_t y,uint16_t colour,char *text);
    extern void vga_clear(uint16_t colour);
    extern void vga_copy(uint16_t *dest,uint16_t x,uint16_t y,uint16_t len);
    extern void vga_paste(uint16_t *src,uint16_t x,uint16_t y,uint16_t len);
    extern void vga_blit_80x25(uint16_t *src);
    extern void cursor_hide(void);
    extern void cursor_show(void);

#   ifdef __cplusplus
        }
#   endif

#endif // VGA_H
