#ifndef __WCACHE__H__
#define  __WCACHE__H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rbtree.h>
#include <linux/timer.h>

/*内容对象结构体*/
typedef struct object {
    char *name;
    void  *data;
    size_t  size;
    char *path;
    unsigned long time;
    struct rb_node node;
}obj;

extern struct rb_root obj_tree;
extern obj *temp_object;

#endif