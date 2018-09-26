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
	{ 0x6144fe8b, __VMLINUX_SYMBOL_STR(netdev_info) },
	{ 0x208614a6, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x5550f2d9, __VMLINUX_SYMBOL_STR(mii_ethtool_gset) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x6466aef4, __VMLINUX_SYMBOL_STR(phy_disconnect) },
	{ 0x57429e24, __VMLINUX_SYMBOL_STR(usbnet_resume) },
	{ 0xbc99cb46, __VMLINUX_SYMBOL_STR(phy_stop) },
	{ 0x459704f4, __VMLINUX_SYMBOL_STR(usbnet_probe) },
	{ 0xdd25bb56, __VMLINUX_SYMBOL_STR(usbnet_set_settings) },
	{ 0x79aa04a2, __VMLINUX_SYMBOL_STR(get_random_bytes) },
	{ 0xa508c431, __VMLINUX_SYMBOL_STR(usbnet_link_change) },
	{ 0x12143024, __VMLINUX_SYMBOL_STR(usbnet_disconnect) },
	{ 0xf7802486, __VMLINUX_SYMBOL_STR(__aeabi_uidivmod) },
	{ 0x7be6fb2d, __VMLINUX_SYMBOL_STR(generic_mii_ioctl) },
	{ 0x1e548afe, __VMLINUX_SYMBOL_STR(phy_ethtool_gset) },
	{ 0x7d598cf9, __VMLINUX_SYMBOL_STR(__dev_kfree_skb_any) },
	{ 0x7e3c0630, __VMLINUX_SYMBOL_STR(usbnet_stop) },
	{ 0xfd67d10b, __VMLINUX_SYMBOL_STR(usbnet_update_max_qlen) },
	{ 0x49fcab7e, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x3e72a097, __VMLINUX_SYMBOL_STR(mdiobus_unregister) },
	{ 0x99ed754e, __VMLINUX_SYMBOL_STR(__netdev_alloc_skb) },
	{ 0x2a407715, __VMLINUX_SYMBOL_STR(phy_start_aneg) },
	{ 0x955b2a34, __VMLINUX_SYMBOL_STR(phy_print_status) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0xbe39c6f9, __VMLINUX_SYMBOL_STR(usbnet_nway_reset) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x1d977d74, __VMLINUX_SYMBOL_STR(phy_start) },
	{ 0xe7482400, __VMLINUX_SYMBOL_STR(mii_nway_restart) },
	{ 0xb6548c36, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0x70bc7c53, __VMLINUX_SYMBOL_STR(mdiobus_free) },
	{ 0xc9870e84, __VMLINUX_SYMBOL_STR(usbnet_get_endpoints) },
	{ 0x73e20c1c, __VMLINUX_SYMBOL_STR(strlcpy) },
	{ 0x45bb6b5c, __VMLINUX_SYMBOL_STR(usbnet_get_drvinfo) },
	{ 0xd262618f, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x7267eeee, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0x6220b4a2, __VMLINUX_SYMBOL_STR(crc32_le) },
	{ 0xc1e41808, __VMLINUX_SYMBOL_STR(usbnet_start_xmit) },
	{ 0xa1932c0c, __VMLINUX_SYMBOL_STR(usbnet_suspend) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x95b9a07e, __VMLINUX_SYMBOL_STR(usbnet_get_link) },
	{ 0x49d5bdf0, __VMLINUX_SYMBOL_STR(usbnet_get_settings) },
	{ 0xe8df35df, __VMLINUX_SYMBOL_STR(skb_copy_expand) },
	{ 0xcecb9887, __VMLINUX_SYMBOL_STR(usbnet_read_cmd) },
	{ 0xcca6874c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0x1bb00d73, __VMLINUX_SYMBOL_STR(mdiobus_register) },
	{ 0xd8f2796c, __VMLINUX_SYMBOL_STR(usbnet_tx_timeout) },
	{ 0xcb7c21c4, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0x26a8c1ae, __VMLINUX_SYMBOL_STR(genphy_resume) },
	{ 0x2b3c25de, __VMLINUX_SYMBOL_STR(usbnet_skb_return) },
	{ 0x428c48fc, __VMLINUX_SYMBOL_STR(usbnet_open) },
	{ 0x20541172, __VMLINUX_SYMBOL_STR(mii_check_media) },
	{ 0x5a92f5ef, __VMLINUX_SYMBOL_STR(usbnet_get_msglevel) },
	{ 0x5082e186, __VMLINUX_SYMBOL_STR(netdev_err) },
	{ 0x67722729, __VMLINUX_SYMBOL_STR(usbnet_unlink_rx_urbs) },
	{ 0x49a2d17e, __VMLINUX_SYMBOL_STR(eth_validate_addr) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x9d669763, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x4d8eb4b8, __VMLINUX_SYMBOL_STR(usbnet_write_cmd_async) },
	{ 0x361c6a40, __VMLINUX_SYMBOL_STR(usbnet_change_mtu) },
	{ 0x8ffb8dda, __VMLINUX_SYMBOL_STR(phy_connect) },
	{ 0x3cc03229, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0x6cd1cf26, __VMLINUX_SYMBOL_STR(phy_mii_ioctl) },
	{ 0x7d6a3df4, __VMLINUX_SYMBOL_STR(phy_ethtool_sset) },
	{ 0xb81960ca, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0x20d83525, __VMLINUX_SYMBOL_STR(mii_link_ok) },
	{ 0x99bb8806, __VMLINUX_SYMBOL_STR(memmove) },
	{ 0x7271a4b5, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0x78c28c10, __VMLINUX_SYMBOL_STR(eth_mac_addr) },
	{ 0x68d9c327, __VMLINUX_SYMBOL_STR(usbnet_write_cmd) },
	{ 0xbbb378d6, __VMLINUX_SYMBOL_STR(usbnet_set_msglevel) },
	{ 0xed7e8a05, __VMLINUX_SYMBOL_STR(mdiobus_alloc_size) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=usbnet";

MODULE_ALIAS("usb:v077Bp2226d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0846p1040d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p1A00d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p1720d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v07B8p420Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v08DDp90FFd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0557p2009d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0411p003Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0411p006Ed*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v6189p182Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0DF6p0056d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0DF6p061Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v07AAp0017d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1189p0893d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1631p6200d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04F1p3008d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v17EFp7203d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p772Bd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p7720d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p1780d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0789p0160d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v13B1p0018d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1557p7720d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v07D1p3C05d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p3C05d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p1A02d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1737p0039d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04BBp0930d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v050Dp5055d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05ACp1402d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p772Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v14EApAB11d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0DB0pA877d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p7E2Bd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B95p172Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v066Bp20F9d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "FE714F923595234F122F542");
