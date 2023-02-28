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
	{ 0xd2138d25, "sysfs_remove_group" },
	{ 0x999e8297, "vfree" },
	{ 0x37a0cba, "kfree" },
	{ 0xe1537255, "__list_del_entry_valid" },
	{ 0x854e4362, "kobject_put" },
	{ 0x4bb0c47b, "sysfs_create_group" },
	{ 0xb65e1731, "kobject_create_and_add" },
	{ 0xf6924e4, "kernel_kobj" },
	{ 0x5635a60a, "vmalloc_user" },
	{ 0x68f31cbd, "__list_add_valid" },
	{ 0xc5850110, "printk" },
	{ 0xa3a8e609, "kmem_cache_alloc_trace" },
	{ 0xc730d3eb, "kmalloc_caches" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "E160FCD396AE666D5D4F1F9");
