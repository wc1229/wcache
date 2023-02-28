/*
 * file name:buffer.c
 */
#include<linux/module.h>

#include<linux/init.h>

#include<linux/vmalloc.h>
//#include<linux/moduleparam.h>

MODULE_AUTHOR("wc1229");
MODULE_LICENSE("GPL");

//static int nbr = 10;
//module_param(nbr,int,S_IRUGO);

#define MEM_VMALLOC_SIZE 8092

char* mem_spvm;

static int __init buffer_init(void)
{
    mem_spvm = (char*)vmalloc(MEM_VMALLOC_SIZE);

    if(mem_spvm == NULL)
    {
        printk("vmalloc failed! \n");
    }
    else
    {
        printk("vmalloc successfully! addr = 0x%lx\n",(unsigned long)mem_spvm);
    }

    return 0;
}

static void __exit buffer_exit(void)
{
    if(mem_spvm != NULL)
    {
        vfree(mem_spvm);
        printk("vfree succed! \n");
    }

    printk("exit ! \n");
}

module_init(buffer_init);
module_exit(buffer_exit);

MODULE_DESCRIPTION("Create a memory buffer with a maximum capacity of 100M in the kernel");
MODULE_ALIAS("buffer_100M");

