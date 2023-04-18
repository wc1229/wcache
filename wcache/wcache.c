/*
 * file name:wcache.c
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/rbtree.h>
#include <linux/netfilter_ipv4.h>
#include "wcache.h"
#include "sysfs.h"
#include "rb_tree.h"
#include "test.h"
#include "hook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("A kernel module to allocate a 100MB cache and store struct objects in a linked list.");

struct rb_root obj_tree = RB_ROOT;
obj *temp_object = NULL;
char *temp_path = NULL;

int __init wcache_init(void)
{
    // test();

    create_sysfs();
    
    nf_register_net_hook(&init_net, &pre_hook);
    nf_register_net_hook(&init_net, &post_hook);
    printk(KERN_INFO "init_hook\n");

    return 0;
}

void __exit wcache_exit(void)
{
    tree_delete();
    // skb_delete(temp_object);
    // free_test();

    free_sysfs();

    nf_unregister_net_hook(&init_net, &pre_hook);
    nf_unregister_net_hook(&init_net, &post_hook);
    printk(KERN_INFO "cleanup_hook\n");
}

module_init(wcache_init);
module_exit(wcache_exit);

