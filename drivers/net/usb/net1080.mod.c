#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xfa985410, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x57429e24, __VMLINUX_SYMBOL_STR(usbnet_resume) },
	{ 0xa1932c0c, __VMLINUX_SYMBOL_STR(usbnet_suspend) },
	{ 0x12143024, __VMLINUX_SYMBOL_STR(usbnet_disconnect) },
	{ 0x459704f4, __VMLINUX_SYMBOL_STR(usbnet_probe) },
	{ 0xb6548c36, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0x3cc03229, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0x6144fe8b, __VMLINUX_SYMBOL_STR(netdev_info) },
	{ 0x68d9c327, __VMLINUX_SYMBOL_STR(usbnet_write_cmd) },
	{ 0xcecb9887, __VMLINUX_SYMBOL_STR(usbnet_read_cmd) },
	{ 0x7406cc0f, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x18a6774, __VMLINUX_SYMBOL_STR(skb_pull) },
	{ 0x4d8eb4b8, __VMLINUX_SYMBOL_STR(usbnet_write_cmd_async) },
	{ 0xc9870e84, __VMLINUX_SYMBOL_STR(usbnet_get_endpoints) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x99bb8806, __VMLINUX_SYMBOL_STR(memmove) },
	{ 0x7271a4b5, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0xd262618f, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x7d598cf9, __VMLINUX_SYMBOL_STR(__dev_kfree_skb_any) },
	{ 0xe8df35df, __VMLINUX_SYMBOL_STR(skb_copy_expand) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=usbnet";

MODULE_ALIAS("usb:v0525p1080d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06D0p0622d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "EFF27B20292EA2953ACFB09");
