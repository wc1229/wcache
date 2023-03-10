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
    tree_delete();

    free_test();

    free_sysfs();

}

module_init(wcache_init);
module_exit(wcache_exit);

