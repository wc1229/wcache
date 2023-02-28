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
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x854e4362, "kobject_put" },
	{ 0x66b2ded8, "kobject_init_and_add" },
	{ 0xa3a8e609, "kmem_cache_alloc_trace" },
	{ 0xc730d3eb, "kmalloc_caches" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xb65ef1e1, "kobject_del" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "ADE19CD5529C9B89E7DD8EA");
