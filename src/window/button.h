/**
 * button.h: buttons
 */
#ifndef BUTTON_H

#   define BUTTON_H

#   include "window/window.h"

#   ifdef __cplusplus
        extern "C" {
#   endif

    extern Window *button_create(Window *parent,
                                 uint16_t x,
                                 uint16_t y,
                                 uint16_t length,
                                 char *text,
                                 void *click_handler);
    extern Window *frame_button_close(Window *parent);
    extern Window *frame_button_resize(Window *parent);

#   ifdef __cplusplus
        }
#   endif

#endif
