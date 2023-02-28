/*
 * file name:link.c
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
//typedef static STA;
//typedef char bool;

struct task_struct *task=NULL;	//内核线程
int cnt=0;
//student_t为待插入链表的节点
typedef struct student{
    int age;
    char name[20];
    char sex;
    struct list_head list;//如果需要插入链表则需要链表结构
}student_t;


typedef struct {
    struct list_head lStu;	//链表头
}class_t;

void fun1(void)
{
    printk("fun1\n");
}
void fun2(void)
{
    printk("fun2\n");
}
int task_thread(void *arg)	//内核线程函数
{
    while(!kthread_should_stop()){	//内核线程停止
    cnt++;
    printk("start thread\n");
    set_current_state(TASK_UNINTERRUPTIBLE);	//睡眠
    printk("schedule\n");
    fun1();
    if(cnt == 2)
    {
    printk("time out\n");
     schedule_timeout(20*HZ);	//20s后调度此线程 
    }else
    {
        printk("no time out\n");
        schedule();	//直接调度
    }
    
     printk("end schedule cnt = %d\n",cnt);
    fun2();
    }
    return 0;
}
static int  mylist_init(void)
{
    printk("my list init\n");
    class_t class;
    int i=0;
    INIT_LIST_HEAD(&class.lStu);
    student_t *stu;
    student_t *pRemain;
    stu = kmalloc(sizeof(student_t )*5,GFP_KERNEL);
    if(!stu)

     printk("malloc failed\n");
  
    for(i=0;i<3;i++)
    {
        (stu+i)->age=100+i+2;
        (stu+i)->sex=i;
        sprintf((stu+i)->name,"stu %d",i);
        INIT_LIST_HEAD(&(stu+i)->list);	 //初始化链表
    }
    for(i=0;i<3;i++)
    {
        list_add_tail(&(stu+i)->list,&class.lStu);//节点插入
    }

      stu->age=101;
    sprintf(stu->name,"first");
    list_for_each_entry(pRemain,&class.lStu,list)
    {
        printk("pRemain->name:%s\n",pRemain->name);
    }
    pRemain = list_entry(class.lStu.next,student_t,list);
    printk("pRemain->name:%s\n",pRemain->name);
    task = kthread_run(task_thread,NULL,"test_task");//创建并运行内核线程
    return 0;
}

static void  mylist_exit(void)
{
    printk("my list exit\n");
    kthread_stop(task);
}

module_init(mylist_init);
module_exit(mylist_exit);
MODULE_LICENSE("GPL");



MODULE_DESCRIPTION("Create a Store object struct");
MODULE_ALIAS("list");
