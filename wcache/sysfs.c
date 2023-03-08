#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include "sysfs.h"

struct kobject *my_cache_kobj;

size_t cache_size = CACHE_SIZE;
size_t used_space = 0;
size_t free_space = CACHE_SIZE;
int obj_count = 0;

/*缓存区空间属性读取*/
ssize_t cache_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", cache_size);
}
/*已使用空间属性读取*/
ssize_t used_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", used_space);
}
/*未使用空间属性读取*/
ssize_t free_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%zu\n", free_space);
}
/*内容对象个数属性读取*/
ssize_t obj_count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", obj_count);
}

/*定义属性*/
struct kobj_attribute cache_size_attr = __ATTR(cache_size, 0444, cache_size_show, NULL);
struct kobj_attribute used_space_attr = __ATTR(used_space, 0444, used_space_show, NULL);
struct kobj_attribute free_space_attr = __ATTR(free_space, 0444, free_space_show, NULL);
struct kobj_attribute obj_count_attr = __ATTR(obj_count, 0444, obj_count_show, NULL);

/*使属性组指向所有属性*/
struct attribute *attrs[] = {
    &cache_size_attr.attr,
    &used_space_attr.attr,
    &free_space_attr.attr,
    &obj_count_attr.attr,
    NULL,
};

/*定义属性组*/
struct attribute_group attr_group = {
    .attrs = attrs,
};


