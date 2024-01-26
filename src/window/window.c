/**
 * window.c: window functions
 */
#define WINDOW_INTERNALS
#include "window/window.h"
#include "os/mouse.h"
#include "window/label.h"
#include "window/button.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t _handles;
char _frame_single[] = { 218, 191, 192, 217, 196, 179 };
char _frame_double[] = { 201, 187, 200, 188, 205, 186 };

void ev_paint_handler(Window *window);

/**
 * @brief create a default window
 * @param parent
 * @return window
 */
Window *window_create_default
(
    Window *parent,
    char *title,
    uint16_t tx,
    uint16_t ty,
    uint16_t width,
    uint16_t height
) {
    return window_create(parent,
                         title,
                         tx,
                         ty,
                         width,
                         height,
                         WF_DEFAULT,
                         WINDOW_DEFAULT_COLOUR&0x0f,
                         WINDOW_DEFAULT_COLOUR>>4,
                         0);
}

/**
 * @brief create a default window
 * @param parent
 * @return window
 */
Window *window_create
(
    Window *parent,
    char *title,
    uint16_t tx,
    uint16_t ty,
    uint16_t width,
    uint16_t height,
    uint16_t flags,
    uint16_t fg,
    uint16_t bg,
    uint16_t fill
) {
    Window *window=os_malloc(sizeof(Window));
    if(window) {
        window->_handle=_handles++;
        window->_parent=parent;
        if(title) {
            window->_title=os_malloc(strlen(title)+1);
            strcpy(window->_title,title);
        }
        window->_coords._tx=tx;
        window->_coords._ty=ty;
        window->_coords._bx=tx+width;
        window->_coords._by=ty+height;
        // minimum size with frame
        window->_min_size._tx=0;
        window->_min_size._ty=0;
        window->_min_size._bx=9;
        window->_min_size._by=3;
        window->_dirty++;
        window->_flags=flags;
        window->_colour=(bg<<4)|fg;
        window->_fill=fill;
        // set some default handlers
        window->_event_handler=_window_default_event_handler;
        window->_mouse_handler=_window_default_mouse_handler;
        window->_ev_paint=_window_default_ev_paint;
        window->_ev_mouse=_window_default_ev_mouse;
        window->_ev_hook=_window_default_ev_hook;
        window->_ev_resized=_window_default_ev_resized;
        window->_ev_close=_window_default_ev_close;
        window->_ev_clicked=_window_default_ev_clicked;
        window->_memory_length=rect_width(&window->_coords)
                              *rect_height(&window->_coords)
                              *2;
        if(window->_memory_length) {
            window->_memory=os_malloc(window->_memory_length);
            if(window->_memory) {
                if(window->_flags&WF_TITLE) {
                    frame_title_label_create(window);
                }
                if(window->_flags&WF_CLOSE) {
                    frame_button_close(window);
                }
                if(window->_flags&WF_RESIZE) {
                    frame_button_resize(window);
                }
                if(parent) {
                    if(!(window->_flags&WF_POPUP)) {
                        window_add_child(parent,window);
                    }
                    else  {
                        window_add_popup(parent,window);
                    }
                }
            }
            else {
                free(window);
                window=0;
                log_e(_eoom);
            }
        }
    }
    else {
        log_e(_eoom);
    }
    return window;
}

/**
 * @brief clicked handler
 * @param window
 */
void _window_default_ev_clicked(Window *window) {
    if(window) {
        log_d(window->_title);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief destroy a window and it's children
 * @param window
 */
void window_destroy(Window *window) {
    if(window) {
        Window *child;

        // destroy children
        while((child=(Window *)dlist_pop(&window->_children))!=0) {
            window_destroy(child);
        }

        if(window->_ev_destroy) {
            window->_ev_destroy(window);
        }

        // free our memory
        if(window->_user_data) {
            free(window->_user_data);
        }
        if(window->_title) {
            free(window->_title);
        }
        if(window->_memory) {
            free(window->_memory);
        }
        free(window);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief clear the window
 * @param window
 */
 void window_clear(Window *window) {
    if(window) {
        uint16_t bg=window->_fill|(window->_colour<<8);
        uint16_t *ptr=(uint16_t *)window->_memory;
        uint16_t cnt,length=window->_memory_length>>1;
        for(cnt=0;cnt<length;++cnt) {
            *ptr++=bg;
        }
    }
    else {
        log_e(_eoom);
    }
}

/**
 * @brief default window handler
 * @param event
 * @return true if event handled
 */
uint16_t _window_default_event_handler(Window *window,Event *event) {
    uint16_t handled=0;

    if(window) {
        if(event) {
            switch(event->_event) {
                case EV_MOUSE: {
                    EvMouse *mouse=(EvMouse *)event;
                    if(rect_within(&window->_coords,mouse->_cx,mouse->_cy)) {
                        // send mouse events to child windows
                        handled=window->_mouse_handler(window,mouse);
                    }
                    break;
                }
                case EV_PAINT: {
                    ev_paint_handler(window);
                    break;
                }
                case EV_TICK: {
                    if(window->_ev_tick) {
                        window->_ev_tick(window,(EvTick *)event);
                    }
                    // then call the children's tick
                    dlist_walk(&window->_children,window_tick_walk,event);
                    break;
                }
                case EV_CLOSE: {
                    EvClose *close=(EvClose *)event;
                    Window *window=close->_window;

                    // no close handler or close handler says ok
                    if(!window->_ev_close||window->_ev_close(window)) {
                        Window *parent=window->_parent;
                        if(parent) {
                            Window *child;
                            uint16_t ok=1;

                            //#@#@ FIX ME - make a walk, also make paint a walk

                            // check/inform our children
                            for(child=(Window *)window->_children._head;
                                child;
                                child=(Window *)((DNode *)child)->_next) {
                                ok&=child->_ev_close
                                        ?child->_ev_close(child)
                                        :0;
                                if(!ok) {
                                    break;
                                }
                            }
                            if(ok) {
                                // and our popups
                                for(child=(Window *)window->_popups._head;
                                    child;
                                    child=(Window *)((DNode *)child)->_next) {
                                    ok&=child->_ev_close
                                            ?child->_ev_close(child)
                                            :0;
                                    if(!ok) {
                                        break;
                                    }
                                }
                            }

                            if(ok) {
                                if(dlist_remove(&parent->_children,
                                                (DNode *)window)
                                 ||dlist_remove(&parent->_popups,
                                                (DNode *)window)) {
                                    uint16_t active=window->_flags&WF_ACTIVE;
                                    window_destroy(window);
                                    window_request_paint(parent);
                                    if(active) {
                                        Window *w;
                                        w=(Window *)dlist_tail(&parent->_children);
                                        if(w) {
                                            w->_flags|=WF_ACTIVE;
                                            window_set_dirty(w);
                                        }
                                    }
                                }
                                else {
                                    log_e("window not found");
                                }
                            }
                        }
                        else {
                            log_e("no parent");
                        }
                    }
                    break;
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
 * @brief default mouse handler
 * @param mouse
 * @return true if event handled
 */
uint16_t _window_default_mouse_handler(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        if(mouse) {
            Window *child;

            mouse->_cx-=window->_coords._tx;
            mouse->_cy-=window->_coords._ty;
            for(child=(Window *)window->_popups._tail;
                !handled&&child;
                child=(Window *)child->_node._last) {
                if(rect_within(&child->_coords,mouse->_cx,mouse->_cy)) {
                    handled=child->_mouse_handler(child,mouse);
                }
            }
            for(child=(Window *)window->_children._tail;
                !handled&&child;
                child=(Window *)child->_node._last) {
                if(rect_within(&child->_coords,mouse->_cx,mouse->_cy)) {
                    handled=child->_mouse_handler(child,mouse);
                }
            }
            if(!handled) {
                if(window->_ev_mouse) {
                    handled=window->_ev_mouse(window,mouse);
                }
            }
            mouse->_cx+=window->_coords._tx;
            mouse->_cy+=window->_coords._ty;
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
 * @brief draw the contents of a window
 * @param window
 */
void _window_default_ev_paint(Window *window) {
    if(window) {
        window_clear(window);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief handle close message
 * @param window
 * @return true
 */
uint16_t _window_default_ev_close(struct s_Window *window) {
    return 1;
}

/**
 * @brief tell children we resized
 * @param window
 */
void _window_default_ev_resized(Window *window) {
    if(window) {
        DNode *node;
        for(node=window->_children._head;node;node=node->_next) {
            Window *child=(Window *)node;
            if(child->_ev_resized) {
                child->_ev_resized(child);
            }
        }
        for(node=window->_popups._head;node;node=node->_next) {
            Window *child=(Window *)node;
            if(child->_ev_resized) {
                child->_ev_resized(child);
            }
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief handle the mouse
 * @param window
 */
uint16_t _window_default_ev_mouse(Window *window,EvMouse *mouse) {
    uint16_t handled=0;

    if(window) {
        if(mouse) {
            if(mouse->_mask&MOUSE_EVT_LB_DOWN) {
                if(!(window->_flags&WF_ACTIVE)) {
                    ++handled;
                    window_set_topmost(window);
                }
                else {
                    if(mouse->_cy==0) {
                        ++handled;
                        window->_flags|=WF_DRAGGING;
                        window->_drag._tx=mouse->_cx;
                        window->_drag._ty=mouse->_cy;
                        event_hook(EV_MOUSE,window);
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
 * @brief get window/mouse delta, subtracting this from the mouse cursor
 *        gives the internal coord
 * @param window
 * @param delta
 */
void window_mouse_delta(Window *window,Rect *delta) {
    if(window) {
        if(delta) {
            Window *parent;
            delta->_tx=window->_coords._tx;
            delta->_ty=window->_coords._ty;
            for(parent=window->_parent;parent;parent=parent->_parent) {
                delta->_tx+=parent->_coords._tx;
                delta->_ty+=parent->_coords._ty;
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
 * @brief default window hook
 * @param event
 * @return true if event handled
 */
uint16_t _window_default_ev_hook(Window *window,Event *event) {
    uint16_t handled=0;

    if(event) {
        switch(event->_event) {
            case EV_MOUSE: {
                EvMouse *mouse=(EvMouse *)event;
                if(mouse->_mask&MOUSE_EVT_MOVE) {
                    Rect delta;
                    int nx,ny;

                    ++handled;
                    window_mouse_delta(window->_parent,&delta);
                    nx=mouse->_cx+delta._tx-window->_drag._tx;
                    ny=mouse->_cy-delta._ty-window->_drag._ty;
                    window_set_pos(window,nx,ny);
                }
                else if(mouse->_mask&MOUSE_EVT_LB_UP) {
                    window->_flags&=~WF_DRAGGING;
                    // unhook the mouse
                    event_unhook(EV_MOUSE,window);
                }
                break;
            }
        }
    }
    else {
        log_e(_enull);
    }
    return handled;
}

/**
 * @brief set window colour
 * @param window
 * @param colour
 */
void window_set_colour(Window *window,uint16_t fg,uint16_t bg) {
    if(window) {
        window->_colour=fg|(bg<<4);
        window_clear(window);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief set background
 * @param window
 * @param colour
 * @param ch
 */
void window_set_background(Window *window,uint16_t ch,uint16_t fg,uint16_t bg) {
    if(window) {
        window->_colour=fg|(bg<<4);
        window->_fill=ch;
        window_clear(window);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief Add a child window
 * @param parent
 * @param child
 */
void window_add_child(Window *parent,Window *child) {
    if(parent) {
        if(child) {
            Window *active=(Window *)dlist_tail(&parent->_children);
            if(active) {
                if(active->_flags&WF_ACTIVE) {
                    active->_flags&=~WF_ACTIVE;
                    active->_dirty++;
                }
                else {
                    log_e("not active?");
                }
            }
            // add the child
            dlist_add(&parent->_children,(DNode *)child);
            child->_parent=parent;
            child->_flags|=WF_ACTIVE;
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
 * @brief Add a popup window
 * @param parent
 * @param child
 */
void window_add_popup(Window *parent,Window *child) {
    if(parent) {
        if(child) {
            // add the child
            dlist_add(&parent->_popups,(DNode *)child);
            child->_parent=parent;
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
 * @brief paint event handler
 * @param window
 */
void ev_paint_handler(Window *window) {
    if(window) {
        // call our paint
        if(window->_dirty&&window->_ev_paint) {
            window->_dirty=0;
            window->_ev_paint(window);
        }
        // then call the children's paint
        dlist_walk(&window->_children,window_paint_walk,window);
        if(window->_flags&WF_FRAME) {
            window_paint_frame(window);
        }
        if(window->_flags&WF_SHADOW) {
            window_shadow_on_parent(window);
        }
        // then call the popup's paint
        dlist_walk(&window->_popups,window_paint_walk,window);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief walk a list of windows and paint them
 * @param node
 * @param data
 */
void window_paint_walk(DNode *node,void *data) {
    if(node) {
        if(data) {
            Window *window=(Window *)node;
            // call our paint
            if(window->_dirty&&window->_ev_paint) {
                window->_dirty=0;
                window->_ev_paint(window);
            }
            // then call the children's paint
            dlist_walk(&window->_children,window_paint_walk,window);
            if(window->_flags&WF_FRAME) {
                window_paint_frame(window);
            }
            if(window->_flags&WF_SHADOW) {
                window_shadow_on_parent(window);
            }
            // then call the popups's paint
            dlist_walk(&window->_popups,window_paint_walk,window);
            window_copy_contents(window);
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
 * @brief walk a list of windows and tick them
 * @param node
 * @param data
 */
void window_tick_walk(DNode *node,void *data) {
    if(node) {
        if(data) {
            Window *window=(Window *)node;
            // call our tick
            if(window->_ev_tick) {
                window->_ev_tick(window,data);
            }
            // then call the children's tick
            dlist_walk(&window->_children,window_tick_walk,data);
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
 * @brief copy the contents of a child onto the parent
 * @param child
 */
void window_copy_contents(Window *child) {
    if(child) {
        Window *parent=child->_parent;
        if(parent) {
            Rect clip=child->_coords;
            Rect boundry;
            boundry._tx=boundry._ty=0;
            boundry._bx=rect_width(&parent->_coords);
            boundry._by=rect_height(&parent->_coords);
            rect_clip(&clip,&boundry);
            // visible
            if(rect_valid(&clip)) {
                uint16_t width=rect_width(&clip)*2;
                uint16_t height=rect_height(&clip);
                uint16_t *src=(uint16_t *)child->_memory;
                uint16_t *dst=(uint16_t *)parent->_memory;
                uint16_t cwidth=rect_width(&child->_coords);
                uint16_t pwidth=rect_width(&parent->_coords);
                uint16_t lines;

                src+=clip._tx-child->_coords._tx
                    +(clip._ty-child->_coords._ty)*cwidth;
                dst+=child->_coords._tx+child->_coords._ty*pwidth;
                for(lines=0;lines<height;++lines) {
                    memcpy(dst,src,width);
                    dst+=pwidth;
                    src+=cwidth;
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
}

/**
 * @brief copy the contents of a parent onto the child
 * @param child
 */
void window_copy_parent_contents(Window *child) {
    if(child) {
        Window *parent=child->_parent;
        if(parent) {
            Rect clip=child->_coords;
            Rect boundry;
            boundry._tx=boundry._ty=0;
            boundry._bx=rect_width(&parent->_coords);
            boundry._by=rect_height(&parent->_coords);
            rect_clip(&clip,&boundry);
            // visible
            if(rect_valid(&clip)) {
                uint16_t *src=(uint16_t *)parent->_memory;
                uint16_t *dst=(uint16_t *)child->_memory;
                uint16_t height=rect_height(&clip);
                uint16_t width=rect_width(&clip);
                uint16_t cwidth=rect_width(&child->_coords);
                uint16_t pwidth=rect_width(&parent->_coords);
                uint16_t lines;
                src+=rect_width(&parent->_coords)*clip._ty+clip._tx;
                for(lines=0;lines<height;++lines) {
                    memcpy(dst,src,width<<1);
                    dst+=cwidth;
                    src+=pwidth;
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
}

/**
 * @brief invoke the event handler
 * @param window
 * @param event
 * @return true if event handled by the window
 */
uint16_t window_event_handler(Window *window,Event *event) {
    return window?window->_event_handler(window,event):0;
}

/**
 * @brief request to paint this window
 * @param window
 */
void window_request_paint(Window *window) {
    if(window) {
        ++window->_dirty;
    }
    else {
        log_e(_enull);
    }
    event_post(event_initialise(EV_PAINT));
}

/**
 * @brief set a handler
 * @param window
 * @param event
 * @param handler
 */
void window_set_handler(Window *window,uint16_t event,void *handler) {
    if(window) {
        switch(event) {
            case EV_MOUSE: {
                window->_ev_mouse=handler;
                break;
            }
            case EV_PAINT: {
                window->_ev_paint=handler;
                break;
            }
            case EV_TICK: {
                window->_ev_tick=handler;
                break;
            }
            case EV_HOOK: {
                window->_ev_hook=handler;
                break;
            }
            case EV_RESIZED: {
                window->_ev_resized=handler;
                break;
            }
            case EV_DESTROY: {
                window->_ev_destroy=handler;
                break;
            }
            case EV_CLOSE: {
                window->_ev_close=handler;
                break;
            }
            case EV_CLICKED: {
                window->_ev_clicked=handler;
                break;
            }
            default: {
                char text[30];
                sprintf(text,"unhandled event %d",event);
                log_e(text);
                break;
            }
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief print some text on a window
 * @param window
 * @param x
 * @param y
 * @param text
 */
void window_print(Window *window,uint16_t x,uint16_t y,char *text) {
    if(window) {
        if(text) {
            Rect clip,r;
            clip._tx=x;
            clip._ty=y;
            clip._bx=x+strlen(text);
            clip._by=y;
            r._tx=r._ty=0;
            r._bx=rect_width(&window->_coords);
            r._by=rect_height(&window->_coords);
            rect_clip(&clip,&r);
            if(rect_valid(&clip)) {
                int16_t i,len=rect_width(&clip);
                char *ptr=(char *)window->_memory;
                ptr+=x*2+y*rect_width(&window->_coords)*2;
                // this is wrong, starts at 0, should take into account clipping
                for(i=0;i<len&&*text;++i,++ptr) {
                    *ptr++=*text++;
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
}

/**
 * @brief print some text on a window
 * @param window
 * @param x
 * @param y
 * @param fg
 * @param bg
 * @param text
 */
void window_print_colour
(
    Window *window,
    uint16_t x,
    uint16_t y,
    uint16_t fg,
    uint16_t bg,
    char *text
) {
    if(window) {
        if(text) {
            Rect clip,r;
            clip._tx=x;
            clip._ty=y;
            clip._bx=x+strlen(text);
            clip._by=y;
            r._tx=r._ty=0;
            r._bx=rect_width(&window->_coords);
            r._by=rect_height(&window->_coords);
            rect_clip(&clip,&r);
            if(rect_valid(&clip)) {
                int16_t i,len=rect_width(&clip);
                char *ptr=(char *)window->_memory;
                ptr+=x*2+y*rect_width(&window->_coords)*2;
                fg|=bg<<4;
                fg&=0x0ff;
                // this is wrong, starts at 0, should take into account clipping
                for(i=0;i<len&&*text;++i) {
                    *ptr++=*text++;
                    *ptr++=fg;
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
}

/**
 * @brief set the user data
 * @param window
 * @param data
 */
void window_set_user_data(Window *window,void *data) {
    if(window) {
        window->_user_data=data;
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief get the user data
 * @param window
 */
void *window_get_user_data(Window *window) {
    return window?window->_user_data:0;
}

/**
 * @brief set the dirty flag
 * @param window
 */
void window_set_dirty(Window *window) {
    if(window) {
        ++window->_dirty;
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief get a window's coordinates
 * @param window
 * @param rect
 */
void window_get_coords(Window *window,Rect *rect) {
    if(window) {
        if(rect) {
            memcpy(rect,&window->_coords,sizeof(Rect));
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
 * @brief create a shadow in around the window edges
 * @param window
 */
void window_shadow(Window *window) {
    if(window) {
        uint16_t w=rect_width(&window->_coords);
        uint16_t h=rect_height(&window->_coords);
        uint16_t i;
        uint8_t *ptr;

        // right y
        ptr=window->_memory+(w<<2)-1;
        for(i=1;i<h;++i) {
            *ptr&=0x0f;
            ptr+=w<<1;
        }
        // bottom x
        ptr=window->_memory+(h-1)*(w<<1)+3;
        for(i=1;i<w-1;++i) {
            *ptr&=0x0f;
            ptr+=2;
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief cast a shadow on the parent
 * @param window
 */
void window_shadow_on_parent(Window *window) {
    if(window) {
        Window *parent=window->_parent;
        if(parent) {
            Rect h,v,boundry=parent->_coords;
            uint16_t width=rect_width(&parent->_coords);

            h._tx=window->_coords._tx+1;
            h._bx=h._tx+rect_width(&window->_coords);
            h._ty=window->_coords._by;
            h._by=h._ty+1;

            v._tx=window->_coords._bx;
            v._bx=v._tx+1;
            v._ty=window->_coords._ty+1;
            v._by=v._ty+rect_height(&window->_coords);

            boundry._by-=2;
            rect_clip(&h,&boundry);
            boundry._by+=2;
            boundry._bx--;
            rect_clip(&v,&boundry);

            if(rect_valid(&h)) {
                uint8_t *ptr=parent->_memory+h._ty*width*2+h._tx*2+1;
                uint16_t i,rw=rect_width(&h);
                for(i=0;i<rw;++i,ptr+=2) {
                    *ptr&=0x0f;
                }
            }

            if(rect_valid(&v)) {
                uint8_t *ptr=parent->_memory+v._ty*width*2+v._tx*2+1;
                uint16_t i,rh=rect_height(&v)-1;
                for(i=0;i<rh;++i,ptr+=2*width) {
                    *ptr&=0x0f;
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
}

/**
 * @brief set the minimum size
 * @param window
 * @param rect
 */
void window_set_minimum(Window *window,Rect *rect) {
    if(window&&rect) {
        uint16_t resize=0;
        uint16_t w=rect_width(&window->_coords);
        uint16_t h=rect_height(&window->_coords);
        memcpy(&window->_min_size,rect,sizeof(Rect));
        if(w<rect_width(&window->_min_size)) {
            ++resize;
            w=rect_width(&window->_min_size);
        }

        if(h<rect_height(&window->_min_size)) {
            ++resize;
            h=rect_height(&window->_min_size);
        }
        if(resize) {
            window_resize(window,w,h);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief
 * @param window
 * @param w
 * @param h
 */
void window_resize(Window *window,uint16_t w,uint16_t h) {
    if(window) {
        if(w<window->_min_size._bx) {
            w=window->_min_size._bx;
        }
        if(h<window->_min_size._by) {
            h=window->_min_size._by;
        }
        if(w!=rect_width(&window->_coords)||h!=rect_height(&window->_coords)) {
            window->_coords._bx=window->_coords._tx+w;
            window->_coords._by=window->_coords._ty+h;
            free(window->_memory);
            window->_memory_length=w*h*2;
            window->_memory=os_malloc(window->_memory_length);
            if(window->_memory) {
                if(window->_parent) {
                    window_set_dirty(window->_parent);
                }
            }
            else {
                log_e(_eoom);
            }
            // let children know we changed
            if(window->_ev_resized) {
                window->_ev_resized(window);
            }
            // do a full paint
            window_request_paint(window);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief set a window to the top
 * @param window
 * @return true if we changed z order
 */
uint16_t window_set_topmost(Window *window) {
    uint16_t z=0;
    if(window) {
        if(!(window->_flags&WF_ACTIVE)) {
            if(window->_parent) {
                Window *parent=window->_parent;
                DNode *child;
                Window *active=(Window *)dlist_tail(&parent->_children);

                if(active) {
                    if(active->_flags&WF_ACTIVE) {
                        active->_flags&=~WF_ACTIVE;
                        active->_dirty++;
                    }
                    else {
                        log_e("not active?");
                    }
                }
                child=dlist_remove(&parent->_children,(DNode *)window);
                if(child) {
                    ++z;
                    dlist_add(&parent->_children,(DNode *)window);
                    window->_flags|=WF_ACTIVE;
                    window->_dirty++;
                    window_request_paint(parent);
                }
                else {
                    log_e("no child");
                    log_i(window->_title);
                    log_i(parent->_title);
                }
            }
        }
    }
    else {
        log_e(_enull);
    }
    return z;
}

/**
 * @brief paint the frame
 * @param window
 */
void window_paint_frame(Window *window) {
    if(window) {
        int i,width,height;
        char *frame=(window->_flags&WF_ACTIVE)
                        ?(window->_flags&WF_DRAGGING)?_frame_single:_frame_double
                        :_frame_single;
        char *ptr=(char *)window->_memory;

        width=rect_width(&window->_coords);
        height=rect_height(&window->_coords);
        *ptr=*(frame+0);
        ptr+=2;
        for(i=0;i<width-2;++i,ptr+=2) {
            *ptr=*(frame+4);
        }
        *ptr=*(frame+1);
        ptr+=2;
        for(i=0;i<height-2;++i) {
            *ptr=*(frame+5);
            ptr+=(width-1)*2;
            *ptr=*(frame+5);
            ptr+=2;
        }
        *ptr=*(frame+2);
        ptr+=2;
        for(i=0;i<width-2;++i,ptr+=2) {
            *ptr=*(frame+4);
        }
        *ptr=*(frame+3);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief set the window's position
 * @param window
 * @param x
 * @param y
 */
void window_set_pos(Window *window,int16_t x,int16_t y) {
    if(window) {
        uint16_t width=rect_width(&window->_coords);
        uint16_t height=rect_height(&window->_coords);
        if(x<0) {
            x=0;
        }
        if(y<0) {
            y=0;
        }
        else if(y>window->_parent->_coords._by-2) {
            y=window->_parent->_coords._by-2;
        }
        window->_coords._tx=x;
        window->_coords._ty=y;
        window->_coords._bx=x+width;
        window->_coords._by=y+height;

        window_set_dirty(window->_parent);
        window_request_paint(window);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief request a window close
 * @param window
 */
void window_request_close(Window *window) {
    if(window) {
        EvClose *close=(EvClose *)event_initialise(EV_CLOSE);
        if(close) {
            close->_handle=window->_handle;
            close->_window=window;
            event_post((Event *)close);
        }
        else {
            log_w(_eoom);
        }
    }
    else {
        log_e(_enull);
    }
}
