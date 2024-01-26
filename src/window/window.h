/**
 * window.h: window functions
 */
#ifndef WINDOW_H

#   define WINDOW_H

#   include "os/os.h"
#   include "window/event.h"
#   include "window/rect.h"
#   include "os/vgacolours.h"

#   define WF_NONE          0x0000
#   define WF_ACTIVE        0x0001
#   define WF_TITLE         0x0002
#   define WF_FRAME         0x0004
#   define WF_CLOSE         0x0008
#   define WF_RESIZE        0x0010
#   define WF_MOVE          0x0020
#   define WF_POPUP         0x0040
#   define WF_SHADOW        0x0080
#   define WF_DRAGGING      0x0100

#   define WF_USER          0x1000
#   define WF_DEFAULT       (WF_TITLE|WF_FRAME|WF_CLOSE|     \
                            WF_RESIZE|WF_MOVE|WF_SHADOW)

#   define WINDOW_DEFAULT_COLOUR    (WHITE|BLUE<<4)
#   define WINFG(colour)            ((colour)&0x0f)
#   define WINBG(colour)            ((colour)>>4)

    typedef struct s_Window {
#       ifdef WINDOW_INTERNALS
        DNode _node;
        uint32_t _handle;
        Rect _coords;
        Rect _min_size;
        uint16_t _flags;
        struct s_Window *_parent;
        char *_title;
        DList _children;
        DList _popups;
        uint16_t _dirty;
        uint16_t _colour;
        uint16_t _fill;
        uint8_t *_memory;
        uint16_t _memory_length;
        Rect _drag;

        // user data that is automatically free'd on destroy
        void *_user_data;

        // handlers
        uint16_t (*_event_handler)(struct s_Window *window,Event *event);
        uint16_t (*_mouse_handler)(struct s_Window *window,EvMouse *mouse);
        uint16_t (*_ev_mouse)(struct s_Window *window,EvMouse *mouse);
        uint16_t (*_ev_hook)(struct s_Window *window,Event *event);
        uint16_t (*_ev_tick)(struct s_Window *window,EvTick *tick);
        void (*_ev_paint)(struct s_Window *window);
        void (*_ev_resized)(struct s_Window *window);
        void (*_ev_destroy)(struct s_Window *window);
        void (*_ev_clicked)(struct s_Window *window);
        uint16_t (*_ev_close)(struct s_Window *window);
#       endif   // WINDOW_INTERNALS
    } Window;


#   ifdef __cplusplus
        extern "C" {
#   endif

    extern Window *window_create_default(Window *parent,
                                         char *title,
                                         uint16_t tx,
                                         uint16_t ty,
                                         uint16_t width,
                                          uint16_t height);
    extern Window *window_create(Window *parent,
                                 char *title,
                                 uint16_t tx,
                                 uint16_t ty,
                                 uint16_t width,
                                 uint16_t height,
                                 uint16_t flags,
                                 uint16_t fg,
                                 uint16_t bg,
                                 uint16_t fill);
    extern void window_destroy(Window *window);
    extern void window_clear(Window *window);
    extern void window_set_colour(Window *window,uint16_t fg,uint16_t bg);
    extern void window_set_background(Window *window,
                                      uint16_t ch,
                                      uint16_t fg,
                                      uint16_t bg);
    extern void window_add_child(Window *parent,Window *child);
    extern void window_add_popup(Window *parent,Window *child);
    extern void window_copy_contents(Window *child);
    extern void window_copy_parent_contents(Window *child);
    extern uint16_t window_event_handler(Window *parent,Event *event);
    extern void window_request_paint(Window *window);
    extern void window_mouse_delta(Window *window,Rect *delta);
    extern void window_set_handler(Window *window,uint16_t event,void *handler);
    extern void window_print(Window *window,uint16_t x,uint16_t y,char *text);
    extern void window_print_colour(Window *window,
                                    uint16_t x,
                                    uint16_t y,
                                    uint16_t fg,
                                    uint16_t bg,
                                    char *text);
    extern void window_set_user_data(Window *window,void *data);
    extern void *window_get_user_data(Window *window);
    extern void window_set_dirty(Window *window);
    extern void window_get_coords(Window *window,Rect *rect);
    extern void window_shadow(Window *window);
    extern void window_shadow_on_parent(Window *window);
    extern void window_set_minimum(Window *window,Rect *rect);
    extern void window_resize(Window *window,uint16_t w,uint16_t h);
    extern uint16_t window_set_topmost(Window *window);
    extern void window_paint_frame(Window *window);
    extern void window_set_pos(Window *window,int16_t x,int16_t y);
    extern void window_request_close(Window *window);

#   ifdef WINDOW_INTERNALS

    extern uint16_t _window_default_event_handler(struct s_Window *window,
                                                  Event *event);
    extern uint16_t _window_default_mouse_handler(struct s_Window *window,
                                                  EvMouse *mouse);
    extern void _window_default_ev_paint(struct s_Window *window);
    extern uint16_t _window_default_ev_close(struct s_Window *window);
    extern uint16_t _window_default_ev_mouse(struct s_Window *window,
                                             EvMouse *mouse);
    extern uint16_t _window_default_ev_hook(Window *window,Event *event);
    extern void _window_default_ev_resized(Window *window);
    extern void _window_default_ev_clicked(Window *window);
    extern void window_paint_walk(DNode *node,void *data);
    extern void window_tick_walk(DNode *node,void *data);

#endif

#   ifdef __cplusplus
        }
#   endif

#endif
