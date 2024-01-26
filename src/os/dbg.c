/*
 * Debug functions
 */
#include "os/dbg.h"
#include "os/vga.h"
#include "lib/list.h"

#include <stdio.h>
#include <stdlib.h>

SList _dbg_stack;
int _dbg_it = 0;
int _dbg_level = 0;
int _dbg_demand = 0;
typedef struct {
    SNode _node;
    int _level;
    char *_text;
} DbgStack;

/**
 * @brief clear the stack
 */
void dbg_stack_clear(void) {
    SNode *node;
    _dbg_demand=_dbg_level=0;
    while((node=slist_pop(&_dbg_stack))!=0) {
        free(((DbgStack *)node)->_text);
        free(node);
    }
}

/**
 * @brief add some text to the stack
 * @param text
 */
void dbg_stack_add(char *text) {
    DbgStack *node=os_malloc(sizeof(DbgStack));
    node->_level=_dbg_level++;
    node->_text=text;
    slist_add(&_dbg_stack,(SNode *)node);
}

/**
 * @brief save the stack
 */
void dbg_stack_save(void) {
    SNode *node=_dbg_stack._head;
    FILE *fp=fopen("stack.txt","at");

    fprintf(fp,"it: %d\n",_dbg_it++);
    while(node) {
        DbgStack *e=(DbgStack *)node;
        fprintf(fp,"\t%d %s\n",e->_level,e->_text);
        node=node->_next;
    }
    fclose(fp);
    dbg_stack_clear();
}

/**
 * @brief demand a save
 */
void dbg_stack_demand_save(void) {
    ++_dbg_demand;
}

/**
 * @brief demand a save
 */
void dbg_stack_save_on_demand(void) {
    if(_dbg_demand) {
        dbg_stack_save();
    }
}

/**
 * @brief Print a uint32 at [x,y]
 * @param x
 * @param y
 * @param i
 */
void print_u32(uint16_t x,uint16_t y,uint32_t i) {
    char text[20];
    sprintf(text,"%04lu",(long)i);
    vga_print(x,y,WHITE,text);
}

/**
 * @brief print a uint16 at [x,y]
 * @param x
 * @param y
 * @param i
 */
void print_u16(uint16_t x,uint16_t y,uint16_t i) {
    char text[20];
    sprintf(text,"%04u",i);
    vga_print(x,y,WHITE,text);
}


/**
 * @brief Print a uint32 at [x,y]
 * @param x
 * @param y
 * @param i
 */
void print_u32x(uint16_t x,uint16_t y,uint32_t i) {
    char text[20];
    sprintf(text,"%04lx",(long)i);
    vga_print(x,y,WHITE,text);
}

/**
 * @brief print a uint16 at [x,y]
 * @param x
 * @param y
 * @param i
 */
void print_u16x(uint16_t x,uint16_t y,uint16_t i) {
    char text[20];
    sprintf(text,"%04x",i);
    vga_print(x,y,WHITE,text);
}

/**
 * @brief open the debug file
 */
void *dbg_fopen(void) {
    static FILE *fp;
    return fp?fp:(fp=fopen("dbg.txt","wt"));
}

#ifndef linux  // hide assembly from linux compilers when warning/error checking

#asm
.text
/**
 * @brief print ax in the top left
 *
 * void print_ax(void)
 */
export _print_ax
_print_ax:
    push    ax
    xor     ax,ax
    push    ax
    push    ax
    call    _print_u16
    pop     ax
    pop     ax
    pop     ax
    ret

/**
 * @brief change the contents of the first character on screen
 *
 * void dbg_do_something(void)
 */
 export _dbg_do_something
 _dbg_do_something:
    push ds
    push si
    push ax

    ! make something happen
    mov     ax,#$b800
    mov     ds,ax
    xor     si,si
    mov     al,[si]
    inc     al
    mov     [si],al

    pop ax
    pop si
    pop ds
    ret
/**
 * @brief change the contents of the first character on screen
 *
 * void dbg_do_something2(void)
 */
 export _dbg_do_something2
 _dbg_do_something2:
    push ds
    push si
    push ax

    ! make something happen
    mov     ax,#$b800
    mov     ds,ax
    xor     si,si
    add     si,#160
    mov     al,[si]
    inc     al
    mov     [si],al

    pop ax
    pop si
    pop ds
    ret

#endif // linux
