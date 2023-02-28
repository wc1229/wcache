#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
 
MODULE_AUTHOR("wc1229");
MODULE_DESCRIPTION("wait queue example");
MODULE_LICENSE("GPL");
 
static int condition;
static struct task_struct *task_2;
 
DECLARE_WAIT_QUEUE_HEAD(wq);
 
//Create a Store object struct

//Store objects written to the buffer

//Read the store object in the buffer
 
static int thread_func_2(void *data)
{
        
    do {
        wait_event_interruptible(wq, condition);
        condition = 0;
        printk(">>>>>this task 2\n");
    }while(!kthread_should_stop());
    printk("2\n");
    
    return 2;
}

static int __init mod_init(void)
{
    condition = 0;
 
    task_2 = kthread_run(thread_func_2, NULL, "thread%d", 2);
    if (IS_ERR(task_2)) {
        printk("******create thread 2 failed\n");
    } else {
        printk("======success create thread 2\n");
    }
 
    printk("wake up task 2\n");
    condition = 1;
    wake_up_interruptible(&wq);

    printk("wake up task 2 again\n");
    condition = 1;
    wake_up_interruptible(&wq);
        
    return 0;
}
 
static void __exit mod_exit(void)
{
    int ret;
 
    if (!IS_ERR(task_2)) {
        ret = kthread_stop(task_2);
        printk("<<<<<<<<task 2 exit, ret = %d\n", ret);
    }
 
    return;
}
 
module_init(mod_init);
module_exit(mod_exit);
   


