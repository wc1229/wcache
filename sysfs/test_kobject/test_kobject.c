/******************************************************************************
 * @file    : test_kobject.c
 * @author  : wuzhihang
 * @date    :
 *
 * @brief   : 测试 kobject的创建和删除
 * history  : init
 ******************************************************************************/
 
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kobject.h>
 
MODULE_LICENSE("GPL");
 
struct kobject* kobj = NULL;
 
static int test_kobject_init(void)
{
    /* 初始化kobject，并加入到sysfs中 */   
    kobj = kobject_create_and_add("test_kobject", NULL);
     
    /* 进入内核模块 */
    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "test_kobject is inited!\n");
    printk(KERN_ALERT "*************************\n");
     
    return 0;
}
 
static void test_kobject_exit(void)
{
    /* 如果 kobj 不为空，则将其从sysfs中删除 */
    if (kobj != NULL)
        kobject_del(kobj);
    /* 退出内核模块 */
    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "test_kobject is exited!\n");
    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "\n\n\n\n\n");
}
 
module_init(test_kobject_init);
module_exit(test_kobject_exit);