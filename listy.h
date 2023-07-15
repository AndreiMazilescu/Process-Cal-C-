#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include "ics.h"

typedef struct node_t {
    Event *val;
    struct node_t  *next;
} node_t;

node_t *new_node(Event *val);
node_t *add_front(node_t *, node_t *);
node_t *add_end(node_t *, node_t *);
node_t *add_inorder(node_t *, node_t *);
node_t *peek_front(node_t *);
node_t *remove_front(node_t *);
void    apply(node_t *, void(*fn)(node_t *, void *), void *arg);

#endif
