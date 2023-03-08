/*
 * file name:wcache.c
 */

// #include <string.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/timer.h>
#include <linux/delay.h> 
#include <linux/rbtree.h>
#include "wcache.h"
#include "sysfs.h"
#include "rb_tree.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("A kernel module to allocate a 100MB cache and store struct objects in a linked list.");

void  *web;
char  web_name[] = "web", web_path[] = "wc1229/path/web";
#define WEB_SIZE (256)

void  *img;
char  img_name[] = "img", img_path[] = "wc1229/path/img";
#define IMG_SIZE (512)

struct rb_root obj_tree = RB_ROOT;

int __init wcache_init(void)
{
    int ret;

    web = vmalloc_user(WEB_SIZE);
    img = vmalloc_user(IMG_SIZE);

    obj_create(web_name, web, WEB_SIZE, web_path);
    msleep(1000);
    obj_create(img_name,  img, IMG_SIZE,  img_path);
    msleep(1000);
    obj_search(img_path);

    my_cache_kobj = kobject_create_and_add("wcache", kernel_kobj);
    if (!my_cache_kobj)
        return -ENOMEM;

    ret = sysfs_create_group(my_cache_kobj, &attr_group);
    if (ret)
        kobject_put(my_cache_kobj);

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

    /*释放图片对象内存*/
    if (img) {
        vfree(img);
        printk(KERN_INFO "img freed successfully using vfree().\n");
    }

    /*释放网页对象内存*/
    if (web) {
        vfree(web);
        printk(KERN_INFO "web freed successfully using vfree().\n");
    }

    sysfs_remove_group(my_cache_kobj, &attr_group);
    kobject_put(my_cache_kobj);
}

module_init(wcache_init);
module_exit(wcache_exit);

