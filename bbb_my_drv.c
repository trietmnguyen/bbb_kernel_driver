#include <linux/module.h>

static int __init my_drv_init(void) {
	       printk("Hello from kernel side.\n");
	              return 0;
}

static void __exit my_drv_exit(void) {
	       printk("Bye bye from kernel side.\n");
}

module_init(my_drv_init);
module_exit(my_drv_exit);

MODULE_DESCRIPTION("Hello world kernel module");
MODULE_AUTHOR("BBB_TrN");
