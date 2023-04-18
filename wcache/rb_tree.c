#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rbtree.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include "sysfs.h"
#include "rb_tree.h"
#include "wcache.h"

/*更新缓存区参数*/
void cache_update(int argc, size_t  size){
    if(argc == ADD){
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

// /*删除skb对象*/
// void skb_delete(obj *object){
//     if(object){
//         /*更新缓存区信息*/
//         cache_update(REDUCE, object->size);
//         /*释放相关内存*/
//         kfree(object->path);
//         kfree_skb(object->skb);
//         rb_erase(&object->node, &obj_tree);
//         kfree(object);
//         printk("The object freed successfully");
//         return;
//     }
//     printk("The object freed failed because of null");
// }

/*删除对象*/
void node_delete(obj *object){
    if(object){
        /*更新缓存区信息*/
        cache_update(REDUCE, object->size);
        /*释放相关内存*/
        if(object->data)kfree(object->data);
        if(object->path)kfree(object->path);
        if(object->name)kfree(object->name);
        rb_erase(&object->node, &obj_tree);
        kfree(object);
        printk("The object freed successfully");
        return;
    }
    printk("The object freed failed because of null");
}

// /*根据路径删除skb*/
// void skb_path_delete(char *path){
//     obj *object = node_search(&obj_tree, path);
//     skb_delete(object);
// }

/*根据路径删除对象*/
void node_path_delete(char *path){
    obj *object = node_search(&obj_tree, path);
    node_delete(object);
}

// /*删除整个skb树*/
// void skb_tree_delete(void){
//     obj *object;
//     struct rb_node *node;
//     for (node = rb_first(&obj_tree); node; node = rb_next(node)){
//         if(rb_entry(node, obj, node)){
//             object = rb_entry(node, obj, node);
//             skb_path_delete(object->path);
//         }
//     }
// }

/*删除整个树*/
void tree_delete(void){
    obj *object;
    struct rb_node *node;
    for (node = rb_first(&obj_tree); node; node = rb_next(node)){
        if(rb_entry(node, obj, node)){
            object = rb_entry(node, obj, node);
            node_path_delete(object->path);
        }
    }
}

/*红黑树LRU查找*/
obj* node_least_recently_used_search(void) {
    obj *object = NULL;
    struct rb_node *node;
    node = rb_first(&obj_tree);
    object = rb_entry(node, obj, node);
    for (node = rb_first(&obj_tree); node; node = rb_next(node)){
        if(object && rb_entry(node, obj, node)->time <= object->time){
            object = rb_entry(node, obj, node);
        }
    }
    return object;
}

/*红黑树LRU删除*/
void node_least_recently_used_delete(void){
    obj *object = node_least_recently_used_search();
    printk(KERN_INFO"Object %s least recently used, delete\n", object->name);
    node_delete(object);
}

/*创建一个内容对象，仅保存路径和名字*/
void obj_start_create(char *path, int path_len) {
    int i;
    /*为内容对象分配内存*/
    if(temp_object)return;
    temp_object = kmalloc(sizeof(obj), GFP_KERNEL);
    temp_object->path = kmalloc(sizeof(char)*(path_len+1), GFP_KERNEL);
    if(!temp_object){
        printk(KERN_INFO"malloc faild\n");
        return;
    }

    /*将内容信息存到对象*/
    for (i = 0; i < path_len; i++)
        temp_object->path[i]  = path[i] ;
    
    temp_object->path[path_len] = '\0';
    temp_object->name = temp_object->path;

    /*将新建的对象添加到列表*/
    if(! node_insert(&obj_tree, temp_object) ){
        printk(KERN_INFO"object %s insert failed",temp_object->name);
    }
    
    printk(KERN_INFO"start create a object; name:%s; path:%s;\n", temp_object->name, temp_object->path);
}

/*保存剩下的空内容：数据、数据大小*/
void obj_end_create(char *data, size_t size) {
    /*为内容对象分配内存*/

    // /*对象内存大于缓存区最大空间，无法缓存*/
    // if(size > cache_size){
    //     // vfree(buffer);
    //     kfree(temp_object);
    //     printk(KERN_INFO"The object %s memory is larger than the buffer space and cannot be cached\n",temp_object->name);
    //     return;
    // }

    // /*判断缓存区是否够用，不够用则执行LRU替换*/
    // while(size > free_space) {
    //     printk(KERN_INFO"Insufficient cache space, Perform LRU replacement\n");
    //     node_least_recently_used_delete();
    // }
    if(!temp_object)return;
    /*将内容信息存到对象*/
    temp_object->size = size;
    temp_object->time = jiffies;

    // /*将新建的对象添加到列表*/
    // if(! node_insert(&obj_tree, temp_object) ){
    //     printk(KERN_INFO"object %s insert failed",temp_object->name);
    // }
    // printk(KERN_INFO"create a object success; name:%s; size:%zu;path:%s;time:%ld\n", temp_object->name, temp_object->size, temp_object->path, temp_object->time);
    // skb_delete(temp_object);
}

/*创建一个内容对象*/
void obj_create(char name[], char *data, int size, char path[]) {
    /*为内容对象分配内存*/
    void *buffer = kmalloc(size, GFP_KERNEL);
    obj *new_obj = kmalloc(sizeof(obj), GFP_KERNEL);
    // kfree(buffer);
    if(!new_obj){
        printk(KERN_INFO"malloc faild\n");
        return;
    }

    // /*对象内存大于缓存区最大空间，无法缓存*/
    // if(size > cache_size){
    //     kfree(buffer);
    //     kfree(path);
    //     kfree(new_obj);
    //     printk(KERN_INFO"The object %s memory is larger than the buffer space and cannot be cached\n",name);
    //     return;
    // }

    // /*判断缓存区是否够用，不够用则执行LRU替换*/
    // while(size > free_space) {
    //     printk(KERN_INFO"Insufficient cache space, Perform LRU replacement\n");
    //     node_least_recently_used_delete();
    // }
    
    /*将内容信息存到对象*/
    memcpy(buffer, data, size);
    new_obj->name = name;
    new_obj->data = buffer;
    new_obj->size = size;
    new_obj->path = path;
    new_obj->time = jiffies;

    /*将新建的对象添加到列表*/
    if(! node_insert(&obj_tree, new_obj) ){
        printk(KERN_INFO"object %s insert failed",name);
    }
    printk(KERN_INFO"create a object success; name:%s; size:%d;path:%s;time:%ld\n", new_obj->name, new_obj->size, new_obj->path, new_obj->time);
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

