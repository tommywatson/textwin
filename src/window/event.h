/**
 * event.h: events
 */
#ifndef EVENT_H

#   define EVENT_H

#   include "lib/list.h"

    typedef struct s_Event {
        SList *_next;
        uint16_t _event;
        uint16_t _handled;
    } Event;

    typedef struct {
        SList *_next;
        uint16_t _event;
        void *_window;
    } EventHook;

           // adjust to largest event
#   define EVENT_MAX_SIZE       sizeof(EvMouse)

    extern Event *event_initialise(uint16_t type);
    extern void event_post(Event *event);
    extern Event *event_read(void);
    extern void event_recycle(Event *event);

    extern void event_hook(uint16_t event,void *window);
    extern void event_unhook(uint16_t event,void *window);
    extern EventHook *event_get_hook(Event *event);

    // Event types, see below for event data
#   define EV_EXIT              0x0001
#   define EV_TICK              0x0002
#   define EV_MOUSE             0x0003
#   define EV_PAINT             0x0004
#   define EV_HOOK              0x0005
#   define EV_RESIZED           0x0006
#   define EV_DESTROY           0x0007
#   define EV_CLOSE             0x0008
#   define EV_CLICKED           0x0009

    // EV_EXIT
    typedef struct {
        Event _event;
    } EvExit;

    // EV_TICK
    typedef struct {
        Event _event;
        uint32_t _tick;
        uint32_t _new_second;
    } EvTick;

    // EV_MOUSE
    typedef struct {
        Event _event;
        uint16_t _mask;
        uint16_t _buttons;
        uint16_t _cx;
        uint16_t _cy;
        uint16_t _hmickey;
        uint16_t _vmickey;
    } EvMouse;

    // EV_PAINT
    typedef struct {
        Event _event;
    } EvPaint;

    // EV_CLOSE
    typedef struct {
        Event _event;
        uint32_t _handle;
        void *_window;
    } EvClose;

#endif
