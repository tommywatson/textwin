/**
 * desktop.c: desktop functionality
 */
#define WINDOW_INTERNALS
#include "window/desktop.h"
#include "os/vga.h"
#include "os/mouse.h"
#include "window/label.h"

#include <stdio.h>

Window *_status;
int _log_type;
int _log_ticks;

void desktop_log_handler(int log_type,char *text) {
    if(_status) {
        if(os_internal_ticks()>_log_ticks
            ||(/*log_type!=LogDbg&&*/log_type<=_log_type)) {
            _log_type=log_type;
            _log_ticks=os_internal_ticks()+5*5-log_type*5;
            switch(log_type) {
                case LogDbg: {
                    window_set_colour(_status,BLACK,LIGHT_GRAY);
                    break;
                }
                case LogInfo:{
                    window_set_colour(_status,GREEN,LIGHT_GRAY);
                    break;
                }
                case LogWarn:{
                    window_set_colour(_status,RED,LIGHT_GRAY);
                    break;
                }
                case LogError:{
                    window_set_colour(_status,YELLOW,RED);
                    break;
                }
            }
            label_set_text(_status,text);
        }
    }
}

/**
 * @brief default desktop event handler
 * @param event
 * @return true if handled
 */
uint16_t _desktop_default_event_handler(Window *desktop,Event *event) {
    uint16_t handled=0;

    if(desktop) {
        if(event) {
            EventHook *hook=event_get_hook(event);
            if(hook) {
                handled=((Window *)hook->_window)->_ev_hook(hook->_window,
                                                            event);
            }
            if(!handled) {
                switch(event->_event) {
                    case EV_PAINT: {
                        // let the default window handler do the painting
                        _window_default_event_handler(desktop,event);
                        // then blit it to the screen
                        mouse_hide();
                        vga_blit_80x25((uint16_t *)desktop->_memory);
                        mouse_show();
                        ++handled;
                        break;
                    }
                    default: {
                        // let the default window handler do the painting
                        handled=_window_default_event_handler(desktop,event);
                        break;
                    }
                }
            }
        }
        else {
            log_e(_enull);
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}


/**
 * @brief create a desktop
 * @return desktop/null
 */
Window *desktop_create(void) {
    Window *desktop=window_create(0,
                                  "Desktop",
                                  0,
                                  0,
                                  80,
                                  25,
                                  WF_NONE,
                                  BLACK,
                                  BLACK,
                                  176);
    if(desktop) {
        Window *w;
        desktop->_event_handler=_desktop_default_event_handler;
        // must be the first window created, window->_handle==1
        w=window_create(desktop,
                        "appwindow",
                        0,
                        1,
                        rect_width(&desktop->_coords),
                        rect_height(&desktop->_coords)-2,
                        WF_NONE,
                        WHITE,
                        LIGHT_GRAY,
                        176);
        w=window_create(desktop,
                        "menubar",
                        0,
                        0,
                        rect_width(&desktop->_coords),
                        1,
                        WF_NONE,
                        BLACK,
                        LIGHT_GRAY,
                        0);
        time_label_create(w,72,0);
        _status=label_create(desktop,
                             0,
                             rect_height(&desktop->_coords)-1,
                             rect_width(&desktop->_coords),
                             "");
        window_set_colour(_status,BLACK,LIGHT_GRAY);
        dbglog_set_handler(desktop_log_handler);
    }
    else {
        log_e(_eoom);
    }
    return desktop;
}

/**
 * @brief get the app window
 * @param window
 */
Window *desktop_get_app_window(Window *window) {
    for(;window;window=window->_parent) {
        if(window->_handle==1) {
            break;
        }
    }
    return window;
}

/**
 * @brief get the desktop (top most) window
 * @param window
 * @return 0/desktop
 */
Window *desktop_get_desktop(Window *window) {
    for(;window&&window->_parent;window=window->_parent);
    return window;
}

/**
 * @brief get the app window
 * @param desktop
 */
Window *desktop_app_window(Window *desktop) {
    /*
    Window *app;
    for(app=(Window *)desktop->_children._head;
        app;
        app=(Window *)((DNode *)app)->_next) {
        if(app->_handle==1) {
            break;
        }
    }
    return app;
    */
    return desktop?(Window *)desktop->_children._head:0;
}
