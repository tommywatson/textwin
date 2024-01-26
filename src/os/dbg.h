/*
 * Debugging functions
 */
#ifndef DBG_H

#   define DBG_H

#   include "os/os.h"

#   ifdef __cplusplus
        extern "C" {
#   endif

    extern void print_u16(uint16_t x,uint16_t y,uint16_t i);
    extern void print_u32(uint16_t x,uint16_t y,uint32_t i);
    extern void print_u16x(uint16_t x,uint16_t y,uint16_t i);
    extern void print_u32x(uint16_t x,uint16_t y,uint32_t i);

    extern void print_ax(void);
    extern void dbg_do_something(void);
    extern void dbg_do_something2(void);

    extern void dbg_stack_clear(void);
    extern void dbg_stack_add(char *text);
    extern void dbg_stack_save(void);
    extern void dbg_stack_demand_save(void);
    extern void dbg_stack_save_on_demand(void);

    extern void *dbg_fopen(void);

#   define dbg_stack_entry(text,number)         \
         {char *t=os_malloc(80);                \
          sprintf(t,"%s %04x",text,number);     \
          dbg_stack_add(t);}

#   ifdef __cplusplus
        }
#   endif

#endif  // DBG_H
