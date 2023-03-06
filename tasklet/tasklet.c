/*
 * file name:tasklet.c
 */

#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");
static unsigned long data=0;
static struct tasklet_struct tasklet;

// 自定义软中断处理函数，在此只有显示功能
static void irq_tasklet_action(unsigned long data)
{
    printk("in the irq_tasklet_action the state of the tasklet is :%ld\n",(&tasklet)->state); //显示中断状态
    printk("tasklet running. by wc\n");
    return;
}

static int   __init let_init(void)
{
    printk("into tasklet_schedule\n");
    tasklet_init(&tasklet, irq_tasklet_action, data);
                  //初始化一个struct tasklet_struct变量，对应一个软中断
    printk("the state of the tasklet after tasklet_init is :%ld\n", (&tasklet)->state);
                  // 显示软中断状态
    tasklet_schedule(&tasklet);                 //将中断变量放入软中断执行队列
    printk("the state of the tasklet after tasklet_schedule is :%ld\n",
  (&tasklet)->state);     //显示中断状态
    printk("out tasklet_schedule_init\n");
    return 0;
}

static void   __exit let_exit(void)
{
    printk("Goodbye tasklet_schedule\n");
    return;
}

module_init(let_init);
module_exit(let_exit);