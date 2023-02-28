/*
 * file name:list.c
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/kmod.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include<linux/init.h>
#include<linux/vmalloc.h>

//定义缓存区空间大小
#define BUFFER_SIZE (100 * 1024 * 1024)
//缓存区空间首地址
char* mem_spvm;
char* mem_update;

//链表节点
typedef struct car_create {
    int name;
    struct list_head list;
}car;

static LIST_HEAD(car_list);

//创建节点
static void car_create(int name) {
    car *wccar = kmalloc(sizeof(car),GFP_KERNEL);
    //  car *wccar = mem_update;
    printk("start create a car");
    if(!wccar){
        printk("malloc faild\n");
    }
    wccar->name = name;
    INIT_LIST_HEAD(&wccar->list);
    list_add_tail(&wccar->list,&car_list);
    // return wccar;
}

static int __init list_init(void){
    car *acar;

    //申请缓存
    // mem_spvm = kmalloc(BUFFER_SIZE,GFP_KERNEL);
    mem_spvm = (char*)vmalloc(BUFFER_SIZE);
    mem_update = mem_spvm;
    if(mem_spvm == NULL){
        printk("vmalloc failed! \n");
    }
    else{
        printk("vmalloc successfully! addr = 0x%lx\n",(unsigned long)mem_spvm);
    }
    
    //创建struct
    car_create(1);
    // car_create(2);
    list_for_each_entry(acar,&car_list,list){
        printk("acar->name:%d\n",acar->name);
        kfree(acar);
    }
    return 0;
}

static void __exit list_exit(void){
    //释放缓存
    if(mem_spvm != NULL){
        vfree(mem_spvm);
        printk("kfree succed! \n");
    }
    printk("car go\n");
}

module_init(list_init);
module_exit(list_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Create a Store object struct");
MODULE_ALIAS("list");


