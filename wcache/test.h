#ifndef __TEST__H__
#define  __TEST__H__

#include <linux/module.h>
#include <linux/kernel.h>

#define STR_LENGTH 10 // 字符串长度
#define DATA 10 // 数据个数

extern void test(void);
extern void free_test(void);

#endif