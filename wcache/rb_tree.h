#ifndef __RB__TREE__H__
#define  __RB__TREE__H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rbtree.h>
#include "wcache.h"

#define FALSE 0
#define TRUE 1

extern int node_delete(struct rb_root root, obj *object);
extern int node_insert(struct rb_root *root, obj *data) ;
extern void obj_create(char name[], void *data, size_t size, char path[]); 
extern void obj_search(char path[]);

#endif