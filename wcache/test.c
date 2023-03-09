#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h> 
#include <linux/vmalloc.h>
#include "rb_tree.h"
#include "test.h"

void  *web;
char  web_name[] = "web", web_path[] = "wc1229/path/web";
#define WEB_SIZE (256)

void  *img;
char  img_name[] = "img", img_path[] = "wc1229/path/img";
#define IMG_SIZE (512)

void test(void){
    web = vmalloc_user(WEB_SIZE);
    img = vmalloc_user(IMG_SIZE);

    obj_create(web_name, web, WEB_SIZE, web_path);
    msleep(1000);
    obj_create(img_name,  img, IMG_SIZE,  img_path);
    msleep(1000);
    obj_search(img_path);
}

void free_test(void){
    /*释放图片对象内存*/
    if (img) {
        vfree(img);
        printk(KERN_INFO "img freed successfully using vfree().\n");
    }

    /*释放网页对象内存*/
    if (web) {
        vfree(web);
        printk(KERN_INFO "web freed successfully using vfree().\n");
    }
}