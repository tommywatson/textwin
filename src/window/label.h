/**
 * label.h: label
 */
#ifndef LABEL_H

#   define LABEL_H

#   include "window/window.h"

#   ifdef __cplusplus
        extern "C" {
#   endif

    Window *label_create(Window *parent,
                         uint16_t x,
                         uint16_t y,
                         uint16_t len,
                         char *text);
    void label_set_text(Window *label,char *text);

    Window *time_label_create(Window *parent,uint16_t x,uint16_t y);
    Window *mouse_label_create(Window *parent,uint16_t x,uint16_t y);
    Window *frame_title_label_create(Window *parent);

#   ifdef __cplusplus
        }
#   endif

#endif
