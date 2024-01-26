/*
 * main.c
 *
 * WARNING: This is by no means usable software, it was born of a few days
 * with some spare time and some 90’s nostalgia.
 * Please see the readme.
 *
 */
#include "os/vga.h"
#include "os/mouse.h"

#define WINDOW_INTERNALS
#include "window/desktop.h"
#include "window/button.h"
#include "window/label.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void create_menu(Window *window);

int main(int argc,char **argv)
{
    uint16_t done=0;
    uint16_t current_tick=0;
    uint32_t current_second=0;
    Window *desktop;
    (void)argc;
    (void)argv;

    // perform init
    os_initialise();
    cursor_hide();
    vga_clear(LIGHT_GRAY);

    // install the mouse and show it
    mouse_handler_install((long)mouse_handler);
    mouse_show();

    // create the desktop
    desktop=desktop_create();
    if(desktop) {
        create_menu(desktop);
    }
    else {  // fail!
        ++done;
    }

    // process the events
    while(!done) {
        Event *event;
        // handle mouse data
        static MouseData data;
        while(mouse_get_data(&data)) {
            EvMouse *mouse=(EvMouse *)event_initialise(EV_MOUSE);
            if(mouse) {
                memcpy(&mouse->_mask,&data,sizeof(MouseData));
                // convert from mickeys to text [x,y]
                mouse->_cx/=8;
                mouse->_cy/=8;
                // post to the event loop
                event_post((Event *)mouse);
            }
            else {
                log_e(_eoom);
            }
        }

        // generate a tick message?
        if(os_internal_ticks()!=current_tick) {
            EvTick *tick=(EvTick *)event_initialise(EV_TICK);
            long ticks=os_internal_ticks();
            current_tick=ticks;
            if(tick) {
                uint32_t t=time(0);
                tick->_tick=ticks;
                if(t!=current_second) {
                    tick->_new_second=current_second=t;
                }
                event_post((Event *)tick);
            }
            else {
                log_e(_eoom);
            }
        }

        // process all the collected events
        while((event=event_read())!=0) {
            if(!window_event_handler(desktop,event)) {
                switch(event->_event) {
                    case EV_EXIT: {
                        ++done;
                        break;
                    }
                }
            }
            // recycle the event
            event_recycle(event);
        }
    }

    mouse_hide();
    mouse_handler_remove();

    if(done) {
        log_i("Done.");
    }

    current_tick+=18;
    while(os_internal_ticks()<current_tick);

    if(desktop) {
        window_set_background(desktop,0,WHITE,BLACK);
        vga_blit_80x25((uint16_t *)desktop->_memory);
        window_destroy(desktop);
    }

    cursor_show();
    os_shutdown();

    return 0;
}

/*
 * Below are the different window types and event handlers
 */

/*
 * Text Window
 */

/**
 * @brief paint the window
 * @param window
 */
void text_window_ev_paint(Window *window) {
    if(window) {
        Rect r;
        int w,h,i;
        char *txt;

        window_clear(window);
        window_get_coords(window,&r);
        txt=os_malloc((w=rect_width(&r))+1);
        if(txt) {
            for(i=0;i<w-2;++i) {
                *(txt+i)='A'+i%60;
            }
            h=rect_height(&r);
            for(i=1;i<h-1;++i) {
                window_print_colour(window,1,i,DARK_GRAY,BLUE,txt);
            }
            free(txt);
        }
        else {
            log_e(_eoom);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief create a text window
 * @param window
 */
void button_text_window(Window *window) {
    if(window) {
        Window *text=window_create_default(desktop_get_app_window(window),
                                           "Text Window",
                                           22,
                                           7,
                                           20,
                                           8);
        if(text) {
            window_set_handler(text,
                               EV_PAINT,
                               text_window_ev_paint);
        }
        else {
            log_e(_eoom);
        }
    }
    else {
        log_e(_enull);
    }
}

/*
 * Calculator window
 */

/**
 * @brief create a calculator
 * @param window
 */
void button_calculator(Window *window) {
    if(window) {
        Window *time=window_create_default(desktop_get_app_window(window),
                                           "Calculator",
                                           22,
                                           7,
                                           20,
                                           8);
        if(time) {
            Rect r;
            r._tx=r._ty=0;
            r._bx=8;
            r._by=3;
        }
        else {
            log_e(_eoom);
        }
    }
    else {
        log_e(_enull);
    }
}

/*
 * Time window
 */

/**
 * @brief create a time window
 * @param window
 */
void create_time_window(Window *window,uint16_t x,uint16_t y,uint16_t flags) {
    if(window) {
        Window *time=window_create(desktop_get_app_window(window),
                                           "Time",
                                           x,
                                           y,
                                           5,
                                           3,
                                           flags,
                                           WINFG(WINDOW_DEFAULT_COLOUR),
                                           WINBG(WINDOW_DEFAULT_COLOUR),
                                           0);
        if(time) {
            Rect r;
            r._tx=r._ty=0;
            r._bx=13;
            r._by=3;
            time_label_create(time,2,1);
            window_set_minimum(time,&r);
        }
        else {
            log_e(_eoom);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief create a time window
 * @param window
 */
void button_time_window(Window *window) {
    if(window) {
        create_time_window(window,22,5,WF_DEFAULT);
    }
    else {
        log_e(_enull);
    }
}

/*
 * Mouse window
 */

/**
 * @brief create a mouse window
 * @param window
 */
void button_mouse_window(Window *window) {
    if(window) {
        Window *mouse=window_create(desktop_get_app_window(window),
                                    "Mouse",
                                    22,
                                    6,
                                    5,
                                    3,
                                    WF_DEFAULT,
                                    WINFG(WINDOW_DEFAULT_COLOUR),
                                    WINBG(WINDOW_DEFAULT_COLOUR),
                                    0);
        if(mouse) {
            Rect r;
            r._tx=r._ty=0;
            r._bx=13;
            r._by=3;
            mouse_label_create(mouse,2,1);
            window_set_minimum(mouse,&r);
        }
        else {
            log_e(_eoom);
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief handle the error button
 * @param window
 */
void button_error(Window *window) {
    time_t t=time(0);
    char text[80];
    sprintf(text,"%lu - this is an error",t);
    log_e(text);
}

/**
 * @brief handle the exit button
 * @param window
 */
void button_exit(Window *window) {
    // goodbye
    event_post(event_initialise(EV_EXIT));
}

/*
 * Menu window
 */

/**
 * @brief create the menu
 * @param desktop
 */
void create_menu(Window *desktop) {
    if(desktop) {
        int w=19,h=20;
        Window *window;

        create_time_window(desktop_app_window(desktop),
                           66,
                           0,
                           WF_DEFAULT&(~(WF_CLOSE|WF_RESIZE)));

        window=window_create(desktop_app_window(desktop),
                             "Menu",
                             3,
                             1,
                             w,
                             h,
                             WF_DEFAULT&(~(WF_CLOSE|WF_RESIZE)),
                             WHITE,//WHITE,
                             LIGHT_GRAY,//BLUE,
                             0);
        if(window) {
            Rect r;
            r._tx=r._ty=0;
            r._bx=w;
            r._by=h;
            window_set_minimum(window,&r);
            button_create(window,2,2,13,"Text Window",button_text_window);
            button_create(window,2,5,13,"Time Window",button_time_window);
            button_create(window,2,8,14,"Mouse Window",button_mouse_window);
            button_create(window,3,11,12,"Calculator",button_calculator);
            button_create(window,5,14,7,"Error",button_error);
            button_create(window,5,17,6,"Exit",button_exit);
        }
        else {
            log_e(_enull);
        }
    }
    else {
        log_e(_enull);
    }
}
