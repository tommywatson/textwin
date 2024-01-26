/*
 * label.c: label funtions
 */
#define WINDOW_INTERNALS
#include "window/label.h"
#include "os/mouse.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief paint the label
 * @param window
 */
void label_ev_paint(Window *window) {
    if(window) {
        window_clear(window);
        window_print(window,0,0,window_get_user_data(window));
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief create a label
 * @param x
 * @param y
 * @param len
 * @param text
 */
Window *label_create
(
    Window *parent,
    uint16_t x,
    uint16_t y,
    uint16_t len,
    char *text
) {
    Window *window=window_create(parent,
                                 text,
                                 x,
                                 y,
                                 len,
                                 1,
                                 WF_NONE,
                                 parent?WINFG(parent->_colour)
                                       :WINFG(WINDOW_DEFAULT_COLOUR),
                                 parent?WINBG(parent->_colour)
                                       :WINBG(WINDOW_DEFAULT_COLOUR),
                                 0);
    if(window) {
        window_set_handler(window,EV_PAINT,label_ev_paint);
        window_set_handler(window,EV_MOUSE,0);
        label_set_text(window,text);
    }
    else {
        log_e(_eoom);
    }
    return window;
}

/**
 * @brief set the text of the label
 * @param label
 * @param text
 */
void label_set_text(Window *label,char *text) {
    if(label) {
        if(text) {
            char *data=window_get_user_data(label);
            if(data) {
                free(data);
            }
            data=os_malloc(strlen(text)+1);
            strcpy(data,text);
            window_set_user_data(label,data);
            window_request_paint(label);
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
 * @brief handle ticks
 * @param window
 * @param tick
 */
uint16_t time_label_tick(Window *window,EvTick *tick) {
    if(tick->_new_second) {
        char text[9];
        time_t t=time(0);
        struct tm *tm=gmtime(&t);
        sprintf(text,"%02d:%02d:%02d",tm->tm_hour,tm->tm_min,tm->tm_sec);
        label_set_text(window,text);
    }
    return 0;
}

/**
 * @brief create a time label
 * @param parent
 * @param x
 * @param y
 * @return label
 */
Window *time_label_create(Window *parent,uint16_t x,uint16_t y) {
    Window *window=label_create(parent,x,y,8,"");
    if(window) {
        EvTick t;
        t._new_second=time(0);
        window_set_handler(window,EV_TICK,time_label_tick);
        time_label_tick(window,&t);
    }
    else {
        log_e(_enull);
    }
    return window;
}

/**
 * @brief handle hook
 * @param window
 * @param event
 */
uint16_t mouse_label_ev_hook(Window *window,Event *event) {
    if(event->_event==EV_MOUSE) {
        EvMouse *mouse=(EvMouse *)event;
        char text[20];
        sprintf(text,
                "%c%c%c[%02d,%02d]",
                mouse->_buttons&MOUSE_LB_DOWN?'L':' ',
                mouse->_mask&MOUSE_EVT_MOVE?'m':' ',
                mouse->_buttons&MOUSE_RB_DOWN?'R':' ',
                mouse->_cx,
                mouse->_cy);
        label_set_text(window,text);
    }
    return 0;
}

/**
 * @brief handle close event
 * @param window
 */
uint16_t mouse_label_ev_close(Window *window) {
    if(window) {
        // unhook the mouse
        event_unhook(EV_MOUSE,window);
    }
    else {
        log_e(_enull);
    }
    return 1;
}

/**
 * @brief create a mouse label
 * @param parent
 * @param x
 * @param y
 * @return label
 */
Window *mouse_label_create(Window *parent,uint16_t x,uint16_t y) {
    Window *window=label_create(parent,x,y,10,"   [  ,  ]");
    if(window) {
        window_set_handler(window,EV_HOOK,mouse_label_ev_hook);
        window_set_handler(window,EV_CLOSE,mouse_label_ev_close);
        // hook the mouse
        event_hook(EV_MOUSE,window);
    }
    else {
        log_e(_enull);
    }
    return window;
}

/**
 * @brief paint the title
 * @param window
 */
void frame_title_ev_paint(Window *window) {
    if(window) {
        char text[80];
        int16_t width=rect_width(&window->_parent->_coords)-2;
        char *title=window_get_user_data(window);
        uint16_t len=strlen(title)+2;

        if(window->_flags&WF_CLOSE) {
            width-=2;
        }
        if(len<width) {
            sprintf(text,"[%s]",title);
        }
        else {
            len=width;
            *text='[';
            memcpy(text+1,title,len-2);
            *(text+len-2)=']';
            *(text+len-1)=0;
        }
        window_clear(window);
        window_print(window,0,0,text);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief paint the title
 * @param window
 */
void frame_title_ev_resized(Window *window) {
    if(window) {
        uint16_t width=rect_width(&window->_parent->_coords)-5;
        uint16_t length=strlen(window->_parent->_title)+2;
        if(length<width) {
            window->_coords._tx=width/2-length/2;
            window->_coords._bx=window->_coords._tx+length;
        }
        else {
            window->_coords._tx=1;
            window->_coords._bx=width;
        }
        window_set_dirty(window);
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief create a frame title label
 * @param parent
 */
Window *frame_title_label_create(Window *parent) {
    Window *window=0;
    if(parent) {
        uint16_t width=rect_width(&parent->_coords);
        uint16_t length=strlen(parent->_title);

        window=window_create(parent,
                             "frame:title",
                             width/2-length/2,
                             0,
                             length+2,
                             1,
                             WF_POPUP,
                             parent->_colour&0x0f,
                             parent->_colour>>4,
                             0);
        if(window) {
            window_set_handler(window,EV_PAINT,frame_title_ev_paint);
            window_set_handler(window,EV_MOUSE,0);
            window_set_handler(window,EV_RESIZED,frame_title_ev_resized);
            label_set_text(window,parent->_title);
        }
    }
    else {
        log_e(_eoom);
    }
    return window;
}

