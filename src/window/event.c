/**
 * event.c: events
 */
#include "window/event.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

SList _events;
SList _events_free;
uint16_t _event_count,_event_free,_event_used;
SList _event_hooks;

/**
 * @brief Initialise an event
 */
Event *event_initialise(uint16_t type) {
    Event *event=(Event *)slist_pop(&_events_free);
    if(!event) {
        event=os_malloc(EVENT_MAX_SIZE);
        if(event) {++_event_count;
            ++_event_used;
            if(!(_event_count&0x0f)) {
                char text[40];
                sprintf(text,
                        "Event count [%u] [f:%u] [u:%u]",
                        _event_count,
                        _event_free,
                        _event_used);
                log_w(text);
            }
        }
        else {
            log_e(_eoom);
        }
    }
    else {
        --_event_free;
    }
    if(event) {
        // reset event and set type
        memset(event,0,EVENT_MAX_SIZE);
        event->_event=type;
    }
    return event;
}

/**
 * @brief Post an event to the event queue
 * @param event
 */
void event_post(Event *event) {
    slist_add(&_events,(SNode *)event);
}

/**
 * @brief read and event from queue
 * @param event
 */
Event *event_read(void) {
   Event *event=(Event *)slist_pop(&_events);
   return event;
}

/**
 * @brief recycle and event once done
 * @param event
 */
void event_recycle(Event *event) {
    ++_event_free;
    slist_push(&_events_free,(SNode *)event);
    /*
    print_u16(30,1,_event_count);
    print_u16(36,1,_event_used);
    print_u16(42,1,_event_free);
     */
}

/**
 * @brief hook an event to a window
 * @param event
 * @param window
 */
void event_hook(uint16_t event,void *window) {
    if(window) {
        EventHook *hook=os_malloc(sizeof(EventHook));
        if(hook) {
            hook->_event=event;
            hook->_window=window;
            // push it on the front
            slist_push(&_event_hooks,(SNode *)hook);
            log_d("Hooked");
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
 * @brief unhook the event
 * @param event
 * @param window
 */
void event_unhook(uint16_t event,void *window) {
    if(window) {
        EventHook *found;
        for(found=(EventHook *)_event_hooks._head;
            found;
            found=(EventHook *)found->_next) {
            if(found->_event==event&&found->_window==window) {
                break;
            }
        }
        if(found) {
            if(slist_remove(&_event_hooks,(SNode *)found)) {
                free(found);
                log_d("Unhooked");
            }
            else {
                log_e("cannot remove hook");
            }
        }
        else {
            log_e("cannot find hook");
        }
    }
    else {
        log_e(_enull);
    }
}

/**
 * @brief Look for an event hook
 * @param event
 * @return hook/0
 */
EventHook *event_get_hook(Event *event) {
    EventHook *found=0;
    EventHook *hook=(EventHook *)_event_hooks._head;
    for(found=0;hook;hook=(EventHook *)hook->_next) {
        if(hook->_event==event->_event) {
            found=hook;
            break;
        }
    }
    return found;
}
