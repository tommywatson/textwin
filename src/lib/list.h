/**
 * list.h: basic list functions
 *
 */
#ifndef LIST_H

#   define LIST_H

#   include "os/os.h"

#   ifdef __cplusplus
        extern "C" {
#   endif

    typedef struct s_SNode {
        struct s_SNode *_next;
    } SNode;

    typedef struct s_SList {
        struct s_SNode *_head;
        struct s_SNode *_tail;
    } SList;

    typedef struct s_DNode {
        struct s_DNode *_last;
        struct s_DNode *_next;
    } DNode;

    typedef struct s_DList {
        struct s_DNode *_head;
        struct s_DNode *_tail;
    } DList;

            // create a list
    extern SList *slist_create(void);
            // add a node to the end of the list
    extern void slist_add(SList *list,SNode *node);
            // push a node onto the beginning
    extern void slist_push(SList *list,SNode *node);
            // remove a node from the list
    extern SNode *slist_remove(SList *list,SNode *node);
            // remove head node
    extern SNode *slist_pop(SList *list);
            // walk a list invoking a callback
    extern void slist_walk(SList *list,
                           void (*walker)(SNode *node,void *data),
                           void *data);

            // create a list
    extern DList *dlist_create(void);
            // add a node to the end of the list
    extern void dlist_add(DList *list,DNode *node);
            // push a node onto the beginning
    extern void dlist_push(DList *list,DNode *node);
            // remove a node from the list
    extern DNode *dlist_remove(DList *list,DNode *node);
            // remove head node
    extern DNode *dlist_pop(DList *list);
            // peek at the tail node
    extern DNode *dlist_tail(DList *list);
                // walk a list invoking a callback
    extern void dlist_walk(DList *list,
                           void (*walker)(DNode *node,void *data),
                           void *data);

#   ifdef __cplusplus
        }
#   endif

#endif
