/**
 * list.c: list functions
 */
#include "lib/list.h"

/**
 * @brief create a slingle linked list
 * @return new list/0
 */
SList *slist_create(void) {
    SList *list=(SList *)os_malloc(sizeof(SList));
    if(list) {
    }
    else {
        log_e(_eoom);
    }
    return list;
}

/**
 * @brief add a node to a single linked list
 * @param node
 */
void slist_add(SList *list,SNode *node) {
    if(list) {
        if(node) {
            node->_next=0;
            if(list->_tail) {
                list->_tail->_next=node;
                list->_tail=node;
            }
            else {
                list->_head=list->_tail=node;
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
 * @brief add a node to a single linked list
 * @param node
 */
void slist_push(SList *list,SNode *node) {
    if(list) {
        if(node) {
            node->_next=list->_head;
            list->_head=node;
            if(!list->_tail) {
                list->_tail=node;
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
 * @brief remove a node deom a single linked list
 * @param node
 */
SNode *slist_remove(SList *list,SNode *node) {
    SNode *found=0;
    if(list) {
        if(node) {
            SNode *last=0;
            for(found=list->_head;found;found=found->_next) {
                if(found==node) {
                    break;
                }
                last=found;
            }
            if(found) {
               if(last) {
                   last->_next=found->_next;
                   if(!last->_next) {
                       list->_tail=last;
                   }
               }
               if(list->_head==found) {
                   list->_head=found->_next;
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
    return found;
}

/**
 * @brief remove a node from a single linked list
 * @param node
 */
SNode *slist_pop(SList *list) {
    SNode *node=0;
    if(list) {
        node=list->_head;
        if(node) {
            list->_head=node->_next;
            if(!list->_head) {
                list->_tail=0;
            }
        }
    }
    else {
        log_e(_enull);
    }
    return node;
}

/**
 * @brief walk a list invoking a callback
 * @param list
 * @param data
 */
void slist_walk(SList *list,void (*walker)(SNode *node,void *data),void *data) {
    if(list) {
        if(walker) {
            SNode *node;
            for(node=list->_head;node;node=node->_next) {
                // invoke the callback
                walker(node,data);
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
 * @brief create a double linked list
 * @return new list/0
 */
DList *dlist_create(void) {
    DList *list=(DList *)os_malloc(sizeof(DList));
    if(list) {
    }
    else {
        log_e(_eoom);
    }
    return list;
}

/**
 * @brief add a node to a double linked list
 * @param node
 */
void dlist_add(DList *list,DNode *node) {
    if(list) {
        if(node) {
            node->_next=0;
            if(list->_tail) {
                list->_tail->_next=node;
                node->_last=list->_tail;
                list->_tail=node;
            }
            else {
                list->_head=list->_tail=node;
                node->_last=0;
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
 * @brief add a node to a double linked list
 * @param node
 */
void dlist_push(DList *list,DNode *node) {
    if(list) {
        if(node) {
            node->_last=0;
            node->_next=list->_head;
            list->_head->_last=node;
            list->_head=node;
            if(!list->_tail) {
                list->_tail=node;
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
 * @brief remove a node from a double linked list
 * @param node
 */
DNode *dlist_remove(DList *list,DNode *node) {
    DNode *found=0;
    if(list) {
        if(node) {
            for(found=list->_head;found;found=found->_next) {
                if(node==found) {
                    break;
                }
            }
            if(found) {
                if(found->_last) {
                    found->_last->_next=found->_next;
                }
                else {
                    list->_head=found->_next;
                    if(!list->_head) {
                        list->_tail=0;
                    }
                }
                if(found->_next) {
                    found->_next->_last=found->_last;
                }
                else {
                    list->_tail=found->_last;
                    if(!list->_tail) {
                        list->_head=0;
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
    return found;
}

/**
 * @brief remove a node from a double linked list
 * @param node
 */
DNode *dlist_pop(DList *list) {
    DNode *node=0;
    if(list) {
        node=list->_head;
        if(node) {
            list->_head=node->_next;
            if(list->_head) {
                list->_head->_last=0;
            }
            else {
                list->_tail=0;
            }
        }
    }
    else {
        log_e(_enull);
    }
    return node;
}

/**
 * @brief walk a list invoking a callback
 * @param list
 * @param data
 */
void dlist_walk(DList *list,void (*walker)(DNode *node,void *data),void *data) {
    if(list) {
        if(walker) {
            DNode *node;
            for(node=list->_head;node;node=node->_next) {
                // invoke the callback
                walker(node,data);
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
 * @brief peek at the tail node
 * @param list
 * @return tail node
 */
DNode *dlist_tail(DList *list) {
    return list?list->_tail:0;
}
