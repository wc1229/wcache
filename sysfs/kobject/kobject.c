#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>

#define BUFFER_SIZE 1024

static struct kobject *my_buffer_kobj;

static int buffer_size = BUFFER_SIZE;
static int used_space = 0;
static int free_space = BUFFER_SIZE;
static int object_count = 0;

static ssize_t buffer_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", buffer_size);
}

static ssize_t used_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", used_space);
}

static ssize_t free_space_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", free_space);
}

static ssize_t object_count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", object_count);
}

static struct kobj_attribute buffer_size_attr = __ATTR(buffer_size, 0444, buffer_size_show, NULL);
static struct kobj_attribute used_space_attr = __ATTR(used_space, 0444, used_space_show, NULL);
static struct kobj_attribute free_space_attr = __ATTR(free_space, 0444, free_space_show, NULL);
static struct kobj_attribute object_count_attr = __ATTR(object_count, 0444, object_count_show, NULL);

static struct attribute *attrs[] = {
    &buffer_size_attr.attr,
    &used_space_attr.attr,
    &free_space_attr.attr,
    &object_count_attr.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static int __init my_buffer_init(void)
{
    int ret;

    my_buffer_kobj = kobject_create_and_add("my_buffer", kernel_kobj);
    if (!my_buffer_kobj)
        return -ENOMEM;

    ret = sysfs_create_group(my_buffer_kobj, &attr_group);
    if (ret)
        kobject_put(my_buffer_kobj);

    return ret;
}

static void __exit my_buffer_exit(void)
{
    sysfs_remove_group(my_buffer_kobj, &attr_group);
    kobject_put(my_buffer_kobj);
}

module_init(my_buffer_init);
module_exit(my_buffer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple buffer sysfs example");
