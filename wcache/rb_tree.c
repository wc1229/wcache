#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rbtree.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "sysfs.h"
#include "rb_tree.h"
#include "wcache.h"

/*更新缓存区参数*/
void cache_update(int argc, size_t  size){
    if(argc){
        used_space += size;
        free_space = cache_size -  used_space;
        obj_count++;
    }
    else{
        used_space -= size;
        free_space = cache_size -  used_space;
        obj_count--;
    }
}

/*红黑树插入数据*/
int node_insert(struct rb_root *root, obj *object) {
    struct rb_node **new = &(root->rb_node), *parent = NULL;
    /* 查询放置节点的位置 */
    while (*new) {
        obj *this = container_of(*new, obj, node);
        int result = strcmp(object->path, this->path);
        parent = *new;
        if (result < 0)
            new = &((*new)->rb_left);
        else if (result > 0)
            new = &((*new)->rb_right);
        else
            return FALSE;
    }
    /* 增加新节点并调整树 */
    rb_link_node(&object->node, parent, new);
    rb_insert_color(&object->node, root);
    /*更新缓存区信息*/
    cache_update(ADD, object->size);
    return TRUE;
}

/*红黑树查找数据*/
obj* node_search(struct rb_root *root, char *path) {
    struct rb_node *node = root->rb_node;
    while (node) {
        obj *data = container_of(node, obj, node);
        int result;
        result = strcmp(path, data->path);
        if (result < 0)
            node = node->rb_left;
        else if (result > 0)
            node = node->rb_right;
        else
            return data;
    }
    return NULL;
}

/*红黑树删除数据*/
void node_delete(char *path){
    obj *object = node_search(&obj_tree, path);
    if(object){
        vfree(object->data);
        rb_erase(&object->node, &obj_tree);
        kfree(object);
        used_space -= object->size;
        /*更新缓存区信息*/
        cache_update(REDUCE, object->size);
    }
}

/*红黑树LRU查找*/
obj* node_least_recently_used_search(void) {
    obj *object = NULL;
    struct rb_node *node;
    for (node = rb_first(&obj_tree); node; node = rb_next(node)){
        if(object && rb_entry(node, obj, node)->time < object->time){
            object = rb_entry(node, obj, node);
        }
    }
    return object;
}

/*红黑树LRU删除*/
int node_least_recently_used_delete(void){
    obj *object = node_least_recently_used_search();
    if(object){
        vfree(object->data);
        rb_erase(&object->node, &obj_tree);
        kfree(object);
        /*更新缓存区信息*/
        cache_update(REDUCE, object->size);
        return TRUE;
    }
    return FALSE;
}

/*创建一个内容对象*/
void obj_create(char name[], void *data, size_t size, char path[]) {
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
}

void obj_search(char *path){
    /*遍历红黑树，根据路径找到内容对象，输出内容名字，更新访问时间*/
    obj *object;
    object = node_search(&obj_tree, path);
    if(object){
        object->time = jiffies;
        printk("The object exists with the name %s, Access time updated to %ld", object->name, object->time);
        return;
    }
    printk("The object does not exist\n");
}

