#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>

dev_t dev_num;
struct class *my_class;
struct device *my_device;

#define DRIVER_NAME "my_drv"

struct cdev my_cdev = {
	.owner = THIS_MODULE
};

int my_open (struct inode *inod, struct file *f) {
	printk("My driver opened successfully\n");
	return 0;
}

int my_release (struct inode *inod, struct file *f) {
	printk("My driver released successfully\n");
	return 0;
}

ssize_t my_read (struct file *f, char __user *umem, size_t size, loff_t * loff)
{
	printk("My driver is reading\n");
	return 0;
}

ssize_t my_write (struct file *f, const char __user *umem, size_t size, loff_t *loff)
{
	printk("My driver is writing\n");
	return 1;
}

struct file_operations my_fops = {
	.open = my_open,
	.read = my_read,
	.write = my_write,
	.release = my_release,
	.owner = THIS_MODULE
};

static int __init my_drv_init(void) {
	printk("Hello from kernel side.\n");
	int ret = 0;

	// dynamic allocate dev num -> major (dynamic), minor (0)
	ret = alloc_chrdev_region(&dev_num, 0, 1, "my_dev");
	if (ret < 0)
	{
		printk("%s. %d. alloc_chrdev_region Fail. ret = %d\n", __func__, __LINE__, ret);
		goto dev_err;
	}

	// init cdev, register file operation
	cdev_init(&my_cdev, &my_fops);
	ret = cdev_add(&my_cdev, dev_num, 1);
	if (ret < 0)
	{
		printk("%s. %d. cdev_add Fail. ret = %d\n", __func__, __LINE__, ret);
		goto cdev_err;
	}

	//create class
	my_class = class_create(THIS_MODULE, "my_class");
	if (IS_ERR(my_class))
	{
		printk("%s. %d. class_create fail\n", __func__, __LINE__);
		goto class_err;
	}

	//create device -> create driver name in /dev
	my_device = device_create(my_class, NULL, dev_num, NULL, DRIVER_NAME);
	if (IS_ERR(my_device))
	{
		printk("%s. %d. device_create fail\n", __func__, __LINE__);
		goto device_error;
	}

	printk("My driver inits successfully\n");

	return 0;

device_error:
	class_destroy(my_class);
class_err:
	cdev_del(&my_cdev);
cdev_err:
	unregister_chrdev_region(dev_num, 1);
dev_err:
	return ret;
}

static void __exit my_drv_exit(void) {
	device_destroy(my_class, dev_num);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_num, 1);

	printk("Bye bye from kernel side.\n");
}

module_init(my_drv_init);
module_exit(my_drv_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello world kernel module");
MODULE_AUTHOR("BBB_TrN");