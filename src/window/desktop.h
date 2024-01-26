/**
 * desktop.h: desktop window
 */
#ifndef DESKTOP_H

#   define DESKTOP_H

#   include "window/window.h"

#   ifdef __cplusplus
        extern "C" {
#   endif

    extern Window *desktop_create(void);
    extern Window *desktop_get_desktop(Window *window);
    extern Window *desktop_app_window(Window *desktop);
    extern Window *desktop_get_app_window(Window *window);

#   ifdef __cplusplus
        }
#   endif

#endif  // DESKTOP_H
