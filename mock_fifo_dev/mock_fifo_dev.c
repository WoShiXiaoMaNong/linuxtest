#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/device.h>

#define DEMO_NAME "my_demo_fifo_dev"

static char *device_buff;
#define MAX_DEVICE_BUFFER_SIZE 64


static struct device *mydemodrv_device;

static int demodrv_open(struct inode *inode, struct file *file)
{
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);

	printk("%s: major=%d minor=%d\n", __func__, major, minor);

	return 0;
}

static int demodrv_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t demodrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t*ppos)
{
	printk("%s enter\n",__func__);

	int actual_read;
	int max_free;
	int need_read;
	int ret;

	max_free = MAX_DEVICE_BUFFER_SIZE - *ppos;
	need_read = max_free > lbuf ? lbuf : max_free;
	
	if (need_read == 0 )
		dev_warn(mydemodrv_device, "No space for read\n");

	ret = copy_to_user(buf, device_buff + *ppos, need_read);
	if (ret == need_read) {
		return -EFAULT;
	}
	
	actual_read = need_read - ret;
	ppos += actual_read;

	printk("%s, actual_readed=%d, pos=%d\n",__func__, actual_read, *ppos);

	return actual_read;
}

static ssize_t demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	int actual_write;
	int free;
	int need_write;
	int ret;
	
	free = MAX_DEVICE_BUFFER_SIZE - *f_pos;

	need_write = free > count ? count : free;
	if (need_write == 0)
		dev_warn(mydemodrv_device, "No space to write.\n");

	ret = copy_from_user(device_buff + *f_pos, buf, need_write);

	if (ret == need_write){
		return -EFAULT;
	}
	
	actual_write = need_write - ret;

	*f_pos += actual_write;

	printk("%s, actual_wirted=%d, pos=%d\n",__func__, actual_write, *f_pos);
	return actual_write;
}

static const struct file_operations demodrv_fops={
	.owner = THIS_MODULE,
	.open = demodrv_open,
	.release = demodrv_release,
	.read = demodrv_read,
	.write = demodrv_write
};

static struct miscdevice mydemodrv_misc_device ={
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEMO_NAME,
	.fops = &demodrv_fops
};

static int __init simple_char_init(void)
{
	int ret;
	ret = misc_register(&mydemodrv_misc_device);
	if (ret) {
		printk("Failed to register misc device\n");
	}
	
	mydemodrv_device = mydemodrv_misc_device.this_device;
	
	device_buff = kmalloc(sizeof(char) * MAX_DEVICE_BUFFER_SIZE, GFP_KERNEL);

	printk("succeeded register misc char device : %s\n", DEMO_NAME);
	
	return 0;
}

static void __exit simple_char_exit(void)
{
	printk("removing device\n");
	
	misc_deregister(&mydemodrv_misc_device);

	kfree(device_buff);
}



module_init(simple_char_init);
module_exit(simple_char_exit);


MODULE_AUTHOR("Zhong,Ming");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Simple char device");

































