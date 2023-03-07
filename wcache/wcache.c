/*
 * file name:wcache.c
 */

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
// #include <string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("A kernel module to allocate a 100MB cache and store struct objects in a linked list.");

static struct kobject *my_cache_kobj;

#define CACHE_SIZE 1024*1024*100   //定义缓存区空间具体大小
#define FALSE 0
#define TRUE 1

static size_t cache_size = CACHE_SIZE;
static size_t used_space = 0;
static size_t free_space = CACHE_SIZE;
static int obj_count = 0;

/*内容对象结构体*/
typedef struct object {
    char *name;
    void  *data;
    size_t  size;
    char *path;
    unsigned long time;
    struct rb_node node;
}obj;

static void  *web;
static char  web_name[] = "web", web_path[] = "wc1229/path/web";
#define WEB_SIZE (256)

static void  *img;
static char  img_name[] = "img", img_path[] = "wc1229/path/img";
#define IMG_SIZE (512)

// static LIST_HEAD(obj_list);
struct rb_root obj_tree = RB_ROOT;

/*红黑树插入数据*/
int node_insert(struct rb_root *root, obj *data) {
    struct rb_node **new = &(root->rb_node), *parent = NULL;
    /* 查询放置节点的位置 */
    while (*new) {
        obj *this = container_of(*new, obj, node);
        int result = data->time - this->time;
        parent = *new;
        if (result < 0)
            new = &((*new)->rb_left);
        else if (result > 0)
            new = &((*new)->rb_right);
        else
            return FALSE;
    }
    /* 增加新节点并调整树 */
    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, root);
    return TRUE;
}

/*创建一个内容对象*/
static void obj_create(char name[], void *data, size_t size, char path[]) {
    /*为内容对象分配内存*/
    void *buffer = vmalloc(size);
    obj *new_obj = kmalloc(sizeof(obj),GFP_KERNEL);
    printk(KERN_INFO"start create a object\n");
    if(!new_obj){
        printk(KERN_INFO"malloc faild\n");
    }

    /*判断缓存区是否够用*/
    if(size > free_space) {
        kfree(new_obj);
        printk(KERN_INFO"Insufficient cache space, object %s creation failed",name);
        return;
    }
    
    /*将内容信息存到对象*/
    memcpy(buffer, data, size);
    new_obj->name = name;
    new_obj->data = buffer;
    new_obj->size = size;
    new_obj->path = path;
    new_obj->time = jiffies;

    /*将新建的节点添加到列表*/
    if(! node_insert(&obj_tree, new_obj) ){
        printk(KERN_INFO"object %s insert failed",name);
    }
    printk(KERN_INFO"create a object success; name:%s; size:%zu;path:%s;time:%ld\n", new_obj->name, new_obj->size, new_obj->path, new_obj->time);

    /*更新缓存区信息*/
    used_space += new_obj->size;
    free_space = cache_size -  used_space;
    obj_count++;
}

static void obj_callback(char path[]){
    /*遍历链表，根据路径找到内容对象，输出内容名字，更新访问时间*/
    obj *object;
    struct rb_node *node;
    for (node = rb_first(&obj_tree); node; node = rb_next(node)){
        if(!strcmp(path, rb_entry(node, obj, node)->path)){
            object = rb_entry(node, obj, node);
            object->time = jiffies;
            printk("The object exists with the name %s, Access time updated to %ld", object->name, object->time);
            return;
        }
    }
    printk("The object does not exist\n");
}

/*缓存区空间属性读取*/
static ssize_t cache_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", cache_size);
}
/*已使用空间属性读取*/
static ssize_t used_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", used_space);
}
/*未使用空间属性读取*/
static ssize_t free_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", free_space);
}
/*内容对象个数属性读取*/
static ssize_t obj_count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", obj_count);
}

/*定义属性*/
static struct kobj_attribute cache_size_attr = __ATTR(cache_size, 0444, cache_size_show, NULL);
static struct kobj_attribute used_space_attr = __ATTR(used_space, 0444, used_space_show, NULL);
static struct kobj_attribute free_space_attr = __ATTR(free_space, 0444, free_space_show, NULL);
static struct kobj_attribute obj_count_attr = __ATTR(obj_count, 0444, obj_count_show, NULL);

/*使属性组指向所有属性*/
static struct attribute *attrs[] = {
    &cache_size_attr.attr,
    &used_space_attr.attr,
    &free_space_attr.attr,
    &obj_count_attr.attr,
    NULL,
};

/*定义属性组*/
static struct attribute_group attr_group = {
    .attrs = attrs,
};

int __init wcache_init(void)
{
    int ret;

    web = vmalloc_user(WEB_SIZE);
    img = vmalloc_user(IMG_SIZE);

    obj_create(web_name, web, WEB_SIZE, web_path);
    msleep(1000);
    obj_create(img_name,  img, IMG_SIZE,  img_path);
    msleep(1000);
    obj_callback(img_path);

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
            if(object->data)vfree(object->data);
            rb_erase(&object->node, &obj_tree);
            kfree(object);
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

