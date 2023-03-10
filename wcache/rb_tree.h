#ifndef __RB__TREE__H__
#define  __RB__TREE__H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rbtree.h>
#include "wcache.h"

#define FALSE 0
#define TRUE 1

#define REDUCE 0
#define ADD 1

extern void node_delete(obj *object);
extern void node_path_delete(char *path);
extern int node_insert(struct rb_root *root, obj *data);
extern obj* node_search(struct rb_root *root, char *path);
extern void tree_delete(void);
extern obj* node_least_recently_used_search(void);
// extern int node_delete(void);
extern void obj_create(char name[], void *data, size_t size, char path[]); 
extern void obj_search(char path[]);

#endif