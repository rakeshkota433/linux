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
	{ 0x12143024, __VMLINUX_SYMBOL_STR(usbnet_disconnect) },
	{ 0x459704f4, __VMLINUX_SYMBOL_STR(usbnet_probe) },
	{ 0x787e96c1, __VMLINUX_SYMBOL_STR(ethtool_op_get_link) },
	{ 0xbe39c6f9, __VMLINUX_SYMBOL_STR(usbnet_nway_reset) },
	{ 0xbbb378d6, __VMLINUX_SYMBOL_STR(usbnet_set_msglevel) },
	{ 0x5a92f5ef, __VMLINUX_SYMBOL_STR(usbnet_get_msglevel) },
	{ 0xd8f2796c, __VMLINUX_SYMBOL_STR(usbnet_tx_timeout) },
	{ 0x49a2d17e, __VMLINUX_SYMBOL_STR(eth_validate_addr) },
	{ 0xc1e41808, __VMLINUX_SYMBOL_STR(usbnet_start_xmit) },
	{ 0x7e3c0630, __VMLINUX_SYMBOL_STR(usbnet_stop) },
	{ 0x428c48fc, __VMLINUX_SYMBOL_STR(usbnet_open) },
	{ 0xb6548c36, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0x3cc03229, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0xa1932c0c, __VMLINUX_SYMBOL_STR(usbnet_suspend) },
	{ 0x57429e24, __VMLINUX_SYMBOL_STR(usbnet_resume) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xc9870e84, __VMLINUX_SYMBOL_STR(usbnet_get_endpoints) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0xdaa2e9e8, __VMLINUX_SYMBOL_STR(netif_carrier_on) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0xe7482400, __VMLINUX_SYMBOL_STR(mii_nway_restart) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0xfd67d10b, __VMLINUX_SYMBOL_STR(usbnet_update_max_qlen) },
	{ 0x98bafaf0, __VMLINUX_SYMBOL_STR(usbnet_write_cmd_nopm) },
	{ 0x68d9c327, __VMLINUX_SYMBOL_STR(usbnet_write_cmd) },
	{ 0x6220b4a2, __VMLINUX_SYMBOL_STR(crc32_le) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x4d8eb4b8, __VMLINUX_SYMBOL_STR(usbnet_write_cmd_async) },
	{ 0x6144fe8b, __VMLINUX_SYMBOL_STR(netdev_info) },
	{ 0xa508c431, __VMLINUX_SYMBOL_STR(usbnet_link_change) },
	{ 0x9d669763, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x68f6a542, __VMLINUX_SYMBOL_STR(netdev_warn) },
	{ 0x3055e142, __VMLINUX_SYMBOL_STR(usbnet_read_cmd_nopm) },
	{ 0xcecb9887, __VMLINUX_SYMBOL_STR(usbnet_read_cmd) },
	{ 0x7be6fb2d, __VMLINUX_SYMBOL_STR(generic_mii_ioctl) },
	{ 0xa4515a, __VMLINUX_SYMBOL_STR(mii_ethtool_sset) },
	{ 0x5550f2d9, __VMLINUX_SYMBOL_STR(mii_ethtool_gset) },
	{ 0x18a6774, __VMLINUX_SYMBOL_STR(skb_pull) },
	{ 0x2b3c25de, __VMLINUX_SYMBOL_STR(usbnet_skb_return) },
	{ 0x578c842, __VMLINUX_SYMBOL_STR(skb_clone) },
	{ 0x7406cc0f, __VMLINUX_SYMBOL_STR(skb_trim) },
	{ 0x7d598cf9, __VMLINUX_SYMBOL_STR(__dev_kfree_skb_any) },
	{ 0xd262618f, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x3f22414d, __VMLINUX_SYMBOL_STR(pskb_expand_head) },
	{ 0xf7802486, __VMLINUX_SYMBOL_STR(__aeabi_uidivmod) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=usbnet";

MODULE_ALIAS("usb:v0B95p1790d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p178Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p4A00d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0DF6p0072d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04E8pA100d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v17EFp304Bd*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "4C676DD260D26D648107038");
