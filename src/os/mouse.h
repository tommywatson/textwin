/*
 * mouse functions
 */
#ifndef MOUSE_H

#   define MOUSE_H

#   include "os/os.h"

#   define MOUSE_EVT_MOVE      0x01
#   define MOUSE_EVT_LB_DOWN   0x02
#   define MOUSE_EVT_LB_UP     0x04
#   define MOUSE_EVT_RB_DOWN   0x08
#   define MOUSE_EVT_RB_UP     0x10
#   define MOUSE_EVT_MB_DOWN   0x20
#   define MOUSE_EVT_MB_UP     0x40
#   define MOUSE_EVT_ALL       0x7f

#   define MOUSE_LB_DOWN        0x01
#   define MOUSE_RB_DOWN        0x02

    typedef struct {
        uint16_t _mask;
        uint16_t _buttons;
        uint16_t _cx;
        uint16_t _cy;
        uint16_t _hmickey;
        uint16_t _vmickey;
    } MouseData;


#   ifdef __cplusplus
        extern "C" {
#   endif

    extern void mouse_hide(void);
    extern void mouse_show(void);
    extern void mouse_handler(void);
    extern void mouse_handler_install(long handler);
    extern void mouse_handler_remove(void);
    extern int  mouse_data_available(void);
    extern int mouse_get_data(MouseData *data);

#   ifdef __cplusplus
        }
#   endif

#endif  // MOUSE_H
