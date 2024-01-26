/**
 * button.c: buttons
 */
#define WINDOW_INTERNALS
#include "window/button.h"
#include "window/event.h"
#include "os/mouse.h"\

#include <string.h>

char _square[2] = { 0xfe,0 };
char _corner[2] = { 0xd9,0 };

/**
 * @brief handle hooked messages
 * @param window
 * @param mouse
 * @return true if handled
 */
uint16_t button_ev_hook(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        if(mouse->_mask==MOUSE_EVT_LB_UP) {
            Rect delta;
            uint16_t mx,my;

            ++handled;
            window->_flags&=~WF_USER;
            event_unhook(EV_MOUSE,window);
            // we depend on parents to be up to date
            window_set_dirty(window->_parent);
            window_request_paint(window);

            window_mouse_delta(window,&delta);
            mx=mouse->_cx-delta._tx+1;
            my=mouse->_cy-delta._ty;
            if(mx<window->_coords._bx&&my<window->_coords._by) {
                if(window->_ev_clicked) {
                    window->_ev_clicked(window);
                }
            }
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}

/**
 * @brief handle mouse messages
 * @param window
 * @param mouse
 * @return true if handled
 */
uint16_t button_ev_mouse(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        if(mouse->_mask==MOUSE_EVT_LB_DOWN) {
            if(mouse->_cy==0&&mouse->_cx<rect_width(&window->_coords)-1) {
                ++handled;
                window->_flags|=WF_USER;
                event_hook(EV_MOUSE,window);
                // we depend on parents to be up to date
                window_set_dirty(window->_parent);
                window_set_dirty(window);
                if(!window_set_topmost(window->_parent)) {
                    window_request_paint(window);
                }
            }
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}

/**
 * @brief handle paint messages
 * @param window
 * @param mouse
 * @return true if handled
 */
void button_ev_paint(Window *window) {
    if(window) {
        if(window->_parent) {
            uint16_t offs=window->_flags&WF_USER?1:0;
            window_copy_parent_contents(window);
            window_print_colour(window,
                                offs,
                                offs,
                                BLACK,
                                GREEN,
                                window_get_user_data(window));
            // button up?
            if(!offs) {
                window_shadow(window);
            }
        }
        else {
            log_e(_enull);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief Create a button
 * @param parent
 * @param x
 * @param y
 * @param length
 * @param text
 * @return
 */
Window *button_create
(
    Window *parent,
    uint16_t x,
    uint16_t y,
    uint16_t length,
    char *text,
    void *click_handler
) {
    Window *button;

    button=window_create(parent,text,x,y,length+1,2,WF_NONE,BLACK,BLACK,0);
    if(button) {
        int tlen=strlen(text);
        int offs=length/2-tlen/2;
        char *data=os_malloc((offs<<1)+tlen+1);
        if(data) {
            memset(data,' ',tlen+(offs<<1));
            memcpy(data+offs,text,tlen);
            window_set_user_data(button,data);
            window_set_handler(button,EV_MOUSE,button_ev_mouse);
            window_set_handler(button,EV_PAINT,button_ev_paint);
            window_set_handler(button,EV_HOOK,button_ev_hook);
            if(click_handler) {
                window_set_handler(button,EV_CLICKED,click_handler);
            }
        }
        else {
            log_e(_eoom);
        }
    }
    else {
        log_e(_enull);
    }
    return button;
}


/**
 * @brief handle hooked messages
 * @param window
 * @param mouse
 * @return true if handled
 */
uint16_t frame_button_close_ev_hook(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        if(mouse->_mask==MOUSE_EVT_LB_UP) {
            Rect delta;
            uint16_t mx,my;

            ++handled;
            window->_flags&=~WF_USER;
            event_unhook(EV_MOUSE,window);
            window_mouse_delta(window,&delta);
            mx=mouse->_cx-delta._tx;
            my=mouse->_cy-delta._ty;
            if(mx==1&&my==0) {
                window_request_close(window->_parent);
            }
            else {
                window_request_paint(window);
            }
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}

/**
 * @brief handle mouse messages
 * @param window
 * @param mouse
 * @return true if handled
 */
uint16_t frame_button_close_ev_mouse(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        if(mouse->_mask==MOUSE_EVT_LB_DOWN) {
            if(mouse->_cy==0&&mouse->_cx==1) {
                ++handled;
                window->_flags|=WF_USER;
                event_hook(EV_MOUSE,window);
                window_set_topmost(window->_parent);
                window_set_dirty(window);
            }
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}

/**
 * @brief handle paint messages
 * @param window
 * @param mouse
 * @return true if handled
 */
void frame_button_close_ev_paint(Window *window) {
    if(window) {
        if(window->_parent) {
            uint16_t hooked=window->_flags&WF_USER?1:0;
            uint16_t colour=window->_colour;
            window_print_colour(window,
                                0,
                                0,
                                hooked?YELLOW:colour&0x0f,
                                colour>>4,
                                "[ ]");
            window_print_colour(window,1,0,hooked?RED:YELLOW,colour>>4,_square);
        }
        else {
            log_e(_enull);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief handle a resize
 * @param window
 */
void frame_button_close_ev_resize(Window *window) {
    if(window) {
        // tell our children
        _window_default_ev_resized(window);

        // re jigger on the frame
        window->_coords._tx=rect_width(&window->_parent->_coords)-4;
        window->_coords._ty=0;
        window->_coords._bx=window->_coords._tx+3;
        window->_coords._by=window->_coords._ty+1;
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief Frame close button
 * @param parent
 * @return button
 */
Window *frame_button_close(Window *parent) {
    Window *button=0;
    if(parent) {
        Rect r;

        window_get_coords(parent,&r);
        button=window_create(parent,
                             "frame:close",
                             rect_width(&parent->_coords)-4,
                             0,
                             3,
                             1,
                             WF_POPUP,
                             parent->_colour&0x0f,
                             parent->_colour>>4,
                             0);

        window_set_handler(button,EV_MOUSE,frame_button_close_ev_mouse);
        window_set_handler(button,EV_PAINT,frame_button_close_ev_paint);
        window_set_handler(button,EV_HOOK,frame_button_close_ev_hook);
        window_set_handler(button,EV_RESIZED,frame_button_close_ev_resize);
    }
    else {
        log_e(_eoom);
    }
    return button;
}

/**
 * @brief handle paint messages
 * @param window
 * @param mouse
 * @return true if handled
 */
void frame_button_resize_ev_paint(Window *window) {
    if(window) {
        if(window->_parent) {
            uint16_t hooked=window->_flags&WF_USER?1:0;
            uint16_t colour=window->_colour;
            window_print_colour(window,
                                0,
                                0,
                                hooked?YELLOW:colour&0x0f,
                                colour>>4,_corner);
        }
        else {
            log_e(_enull);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief handle mouse messages
 * @param window
 * @param mouse
 * @return true if handled
 */
uint16_t frame_button_resize_ev_mouse(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        if(mouse->_mask==MOUSE_EVT_LB_DOWN) {
            if(mouse->_cy==0&&mouse->_cx==0) {
                ++handled;
                window->_flags|=WF_USER;
                event_hook(EV_MOUSE,window);
                window_set_topmost(window->_parent);
                window_set_dirty(window);
            }
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}

/**
 * @brief handle a resize
 * @param window
 */
void frame_button_resize_ev_resize(Window *window) {
    if(window) {
        // tell our children
        _window_default_ev_resized(window);

        // re jigger on the frame
        window->_coords._tx=rect_width(&window->_parent->_coords)-1;
        window->_coords._ty=rect_height(&window->_parent->_coords)-1;
        window->_coords._bx=window->_coords._tx+1;
        window->_coords._by=window->_coords._ty+1;
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief handle hooked messages
 * @param window
 * @param mouse
 * @return true if handled
 */
uint16_t frame_button_resize_ev_hook(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        ++handled;
        if(mouse->_mask==MOUSE_EVT_MOVE) {
            Rect delta;
            int16_t mx,my;

            window_mouse_delta(window->_parent,&delta);
            mx=mouse->_cx-delta._tx;
            my=mouse->_cy-delta._ty;
            if(mx>0&&my>0) {
                window_resize(window->_parent,mx+1,my+1);
            }
        }
        else if(mouse->_mask==MOUSE_EVT_LB_UP) {
            window->_flags&=~WF_USER;
            event_unhook(EV_MOUSE,window);
            // we depend on parents to be up to date
            window_set_dirty(window->_parent);
            window_request_paint(window);
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}

/**
 * @brief Frame resize button
 * @param parent
 * @return button
 */
Window *frame_button_resize(Window *parent) {
    Window *button=0;
    if(parent) {
        Rect r;

        window_get_coords(parent,&r);
        button=window_create(parent,
                             "frame:resize",
                             rect_width(&parent->_coords)-1,
                             rect_height(&parent->_coords)-1,
                             1,
                             1,
                             WF_POPUP,
                             parent->_colour&0x0f,
                             parent->_colour>>4,
                             0);

        window_set_handler(button,EV_MOUSE,frame_button_resize_ev_mouse);
        window_set_handler(button,EV_PAINT,frame_button_resize_ev_paint);
        window_set_handler(button,EV_HOOK,frame_button_resize_ev_hook);
        window_set_handler(button,EV_RESIZED,frame_button_resize_ev_resize);
    }
    else {
        log_e(_eoom);
    }
    return button;
}
