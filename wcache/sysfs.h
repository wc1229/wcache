#ifndef __SYSFS__H__
#define  __SYSFS__H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>

#define CACHE_SIZE 1024*1024*100   //定义缓存区空间具体大小

extern struct kobject *my_cache_kobj;

extern size_t cache_size;
extern size_t used_space;
extern size_t free_space;
extern int obj_count ;

extern ssize_t cache_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t used_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t free_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
extern ssize_t obj_count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);

/*定义属性*/
extern struct kobj_attribute cache_size_attr;
extern struct kobj_attribute used_space_attr;
extern struct kobj_attribute free_space_attr;
extern struct kobj_attribute obj_count_attr;

/*使属性组指向所有属性*/
extern struct attribute *attrs[];

/*定义属性组*/
extern struct attribute_group attr_group;

#endif