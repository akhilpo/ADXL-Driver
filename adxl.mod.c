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

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x31d8dd22, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x281a481e, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x13dbba68, __VMLINUX_SYMBOL_STR(i2c_unregister_device) },
	{ 0x8c03d20c, __VMLINUX_SYMBOL_STR(destroy_workqueue) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x8e9c7933, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0xf816c866, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x43a53735, __VMLINUX_SYMBOL_STR(__alloc_workqueue_key) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x22938941, __VMLINUX_SYMBOL_STR(i2c_new_dummy) },
	{ 0x3057177d, __VMLINUX_SYMBOL_STR(i2c_get_adapter) },
	{ 0xa4a61ba4, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x8145fc4e, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x366cd732, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x9a0daf20, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xb0c929ed, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x40a4dd71, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x67c2fa54, __VMLINUX_SYMBOL_STR(__copy_to_user) },
	{ 0xc542f9cc, __VMLINUX_SYMBOL_STR(i2c_smbus_read_byte_data) },
	{ 0xb2026c56, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0xf30cb9cb, __VMLINUX_SYMBOL_STR(i2c_smbus_write_byte_data) },
	{ 0xb2d48a2e, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

