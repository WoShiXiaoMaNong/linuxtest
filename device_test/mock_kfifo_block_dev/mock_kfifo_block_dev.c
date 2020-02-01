#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kfifo.h>
#include <linux/wait.h>

//static struct kfifo myfifo;

#define DEMO_NAME "my_demo_kfifo_block_dev"
DEFINE_KFIFO(myfifo, char,64);

struct mydemo_device{
	struct device *dev;
	char *name;
	struct miscdevice *miscdev;
	wait_queue_head_t read_queue;
	wait_queue_head_t write_queue;
};


static struct mydemo_device my_device;


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

	int ret;
	if (kfifo_is_empty(&myfifo)){
		if (file->f_flags & O_NONBLOCK){
			return -EAGAIN;
		}
		printk("%s: pid=%d, going to sleep\n",__func__, current->pid);
		ret = wait_event_interruptible(my_device.read_queue, !kfifo_is_empty(&myfifo));
		if (ret){
			return ret;
		}
	}
	int actual_read;
	ret = kfifo_to_user(&myfifo, buf, lbuf, &actual_read);
	if (ret) 
		return -EIO;
	
	if (!kfifo_is_full(&myfifo)) {
		wake_up_interruptible(&my_device.write_queue);
		
	}

	printk("%s: actual read:%d ,pos=%lld",__func__, actual_read, *ppos);

	return actual_read;
}

static ssize_t demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret;
	if (kfifo_is_full(&myfifo)){
		if (file->f_flags & O_NONBLOCK) {
			return -EAGAIN;
		}

		printk("%s: pid=%d, going to sleep\n",__func__, current->pid);
		ret = wait_event_interruptible(my_device.write_queue, !kfifo_is_full(&myfifo));	
		return ret;
	}
	int actual_write;

	ret = kfifo_from_user(&myfifo, buf, count, &actual_write);
	if ( ret)
		return -EIO;

	if (!kfifo_is_empty(&myfifo)) {
		wake_up_interruptible(&my_device.read_queue);
	}
	printk("%s: actual write:%d, pos:%lld",__func__, actual_write, *f_pos);
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
	my_device.miscdev = &mydemodrv_misc_device;
	my_device.name = DEMO_NAME;

	int ret;
	ret = misc_register(&mydemodrv_misc_device);
	if (ret) {
		printk("Failed to register misc device\n");
	}
	
	my_device.dev = mydemodrv_misc_device.this_device;
	init_waitqueue_head(&my_device.read_queue);
	init_waitqueue_head(&my_device.write_queue);
	

	printk("succeeded register misc char device : %s\n", DEMO_NAME);
	
	return 0;
}

static void __exit simple_char_exit(void)
{
	printk("removing device\n");
	
	misc_deregister(my_device.miscdev);

}



module_init(simple_char_init);
module_exit(simple_char_exit);


MODULE_AUTHOR("Zhong,Ming");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Simple char device");

































