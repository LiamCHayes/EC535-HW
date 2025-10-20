#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
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
__used
__attribute__((section("__versions"))) = {
	{ 0xa16cf51b, "module_layout" },
	{ 0xf48ec189, "single_release" },
	{ 0xc8b8082b, "seq_read" },
	{ 0x40993e6d, "proc_create" },
	{ 0xcf8d2e6f, "kmem_cache_alloc_trace" },
	{ 0xb44e414c, "kmalloc_caches" },
	{ 0x1f6e82a4, "__register_chrdev" },
	{ 0xb4d05a35, "remove_proc_entry" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xbc64391d, "seq_printf" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0x18b697c2, "single_open" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xf4fa543b, "arm_copy_to_user" },
	{ 0xd9ce8f0c, "strnlen" },
	{ 0xb81960ca, "snprintf" },
	{ 0xf9e73082, "scnprintf" },
	{ 0x188ea314, "jiffies_to_timespec64" },
	{ 0x7c32d0f0, "printk" },
	{ 0xdb7305a1, "__stack_chk_fail" },
	{ 0xc499ae1e, "kstrdup" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x97934ecf, "del_timer_sync" },
	{ 0xd9bbd26e, "send_sig_info" },
	{ 0x5f754e5a, "memset" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x526c3a6c, "jiffies" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0x11a13e31, "_kstrtol" },
	{ 0x328a05f1, "strncpy" },
	{ 0xd2e07f51, "pid_task" },
	{ 0x8cb916c6, "find_vpid" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x85df9b6c, "strsep" },
	{ 0x28cc25db, "arm_copy_from_user" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x37a0cba, "kfree" },
	{ 0x2b68bd2f, "del_timer" },
	{ 0x8373a021, "kill_fasync" },
	{ 0x10942ff7, "fasync_helper" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

