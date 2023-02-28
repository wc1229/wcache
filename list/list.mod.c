#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xfcd11cb4, "module_layout" },
	{ 0x37a0cba, "kfree" },
	{ 0x68f31cbd, "__list_add_valid" },
	{ 0xa3a8e609, "kmem_cache_alloc_trace" },
	{ 0xc730d3eb, "kmalloc_caches" },
	{ 0xc5850110, "printk" },
	{ 0xb8b9f817, "kmalloc_order_trace" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "39BBDF88D2D8CB409CD5327");
