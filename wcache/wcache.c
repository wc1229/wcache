/*
 * file name:wcche.c
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("A kernel module to allocate a 100MB buffer and store struct objects in a linked list.");

// Define the buffer information
#define BUFFER_SIZE 1024*1024*100

static struct kobject *my_buffer_kobj;

static size_t buffer_size = BUFFER_SIZE;
static size_t used_space = 0;
static size_t free_space = BUFFER_SIZE;
static int obj_count = 0;

// Define a struct to store object information
typedef struct object {
    char *name;
    void  *data;
    size_t  size;
    char *path;
    struct list_head list;
}obj;

static void  *web;
static char  web_name[] = "web", web_path[] = "wc1229/path/web";
#define WEB_SIZE (256)

static void  *img;
static char  img_name[] = "img", img_path[] = "wc1229/path/img";
#define IMG_SIZE (512)

static LIST_HEAD(obj_list);

//创建节点
static void obj_create(char name[], void *data, size_t size, char path[]) {
    if(size > free_space) return;
    
    obj *new_obj = kmalloc(sizeof(obj),GFP_KERNEL);
    printk(KERN_INFO"start create a object\n");
    if(!new_obj){
        printk(KERN_INFO"malloc faild\n");
    }

    new_obj->name = name;
    new_obj->data = img;
    new_obj->size = size;
    new_obj->path = path;

    INIT_LIST_HEAD(&new_obj->list);
    list_add_tail(&new_obj->list,&obj_list);
    printk(KERN_INFO"create a object success; name:%s; size:%zu;path:%s\n", name, size, path);

    used_space += new_obj->size;
    free_space = buffer_size -  used_space;
    obj_count++;
    printk(KERN_INFO"used_space:%zu\n", used_space);
    printk(KERN_INFO"free_space:%zu\n", free_space);
    printk(KERN_INFO"obj_count:%d\n", obj_count);
}

//创建kobject
static ssize_t buffer_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", buffer_size);
}

static ssize_t used_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", used_space);
}

static ssize_t free_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", free_space);
}

static ssize_t obj_count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", obj_count);
}

static struct kobj_attribute buffer_size_attr = __ATTR(buffer_size, 0444, buffer_size_show, NULL);
static struct kobj_attribute used_space_attr = __ATTR(used_space, 0444, used_space_show, NULL);
static struct kobj_attribute free_space_attr = __ATTR(free_space, 0444, free_space_show, NULL);
static struct kobj_attribute obj_count_attr = __ATTR(obj_count, 0444, obj_count_show, NULL);

static struct attribute *attrs[] = {
    &buffer_size_attr.attr,
    &used_space_attr.attr,
    &free_space_attr.attr,
    &obj_count_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

int __init list_init(void)
{
    int ret;

    web = vmalloc_user(WEB_SIZE);
    img = vmalloc_user(IMG_SIZE);

    obj_create(web_name, web, WEB_SIZE, web_path);
    obj_create(img_name,  img, IMG_SIZE,  img_path);

    my_buffer_kobj = kobject_create_and_add("my_buffer", kernel_kobj);
    if (!my_buffer_kobj)
        return -ENOMEM;

    ret = sysfs_create_group(my_buffer_kobj, &attr_group);
    if (ret)
        kobject_put(my_buffer_kobj);

    return 0;
}

void __exit list_exit(void)
{
    // Free the linked list
    obj *pos, *next;

    list_for_each_entry_safe(pos, next, &obj_list, list) {
        list_del(&pos->list);
        kfree(pos);
        printk(KERN_INFO "Linked list freed successfully.\n");
    }

    // Free the img
    if (img) {
        vfree(img);
        printk(KERN_INFO "img freed successfully using vfree().\n");
    }

    // Free the web
    if (web) {
        vfree(web);
        printk(KERN_INFO "web freed successfully using vfree().\n");
    }

    sysfs_remove_group(my_buffer_kobj, &attr_group);
    kobject_put(my_buffer_kobj);
}

module_init(list_init);
module_exit(list_exit);

