/*
 * file name:wcache.c
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/rbtree.h>
#include "wcache.h"
#include "sysfs.h"
#include "rb_tree.h"
#include "test.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("A kernel module to allocate a 100MB cache and store struct objects in a linked list.");

struct rb_root obj_tree = RB_ROOT;

int __init wcache_init(void)
{
    test();

    create_sysfs();
    
    return 0;
}

void __exit wcache_exit(void)
{
    /*释放树*/
    obj *object;
    struct rb_node *node;
    for (node = rb_first(&obj_tree); node; node = rb_next(node)){
        if(rb_entry(node, obj, node)){
            object = rb_entry(node, obj, node);
            node_delete(object->path);
            printk("The object freed successfully");
        }
    }

    free_test();

    free_sysfs();

}

module_init(wcache_init);
module_exit(wcache_exit);

