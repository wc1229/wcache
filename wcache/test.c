#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h> 
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/random.h>
#include <linux/slab.h>
#include "rb_tree.h"
#include "test.h"

void* data[DATA];

static char* random_string(void){
    char* str = kmalloc(STR_LENGTH + 1, GFP_KERNEL);
    int i;

    get_random_bytes(str, sizeof(str));

    for (i = 0; i < STR_LENGTH; i++)
        str[i]  = str[i] % 26 + 'a';
    
    str[STR_LENGTH] = '\0';

    return str;
}

static size_t random_size(void){
    size_t i;
    get_random_bytes(&i, sizeof(size_t));
    i = i%256;
    return i;
}

void test(void){
    int i;
    size_t size;
    for(i=0; i<DATA; i++){
        size = random_size();
        data[i] = vmalloc(size);
        obj_create(random_string(), data[i], size, random_string());
        msleep(100);
    }
    // obj_search(img_path);
}

void free_test(void){
    int i;
    for(i=0; i<DATA; i++){
        vfree(data[i]);
    }

}