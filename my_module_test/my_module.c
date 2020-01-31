#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kfifo.h>


//static struct kfifo myfifo;

#define DEMO_NAME "my_module"

struct my_device {
	char *name;
	dev_t dev;
	struct cdev *mycdev;
	int signed count;
	wait_queue_head_t read_queue;
	wait_queue_head_t wirte_queue;	

};

static struct my_device *mydevice;
DEFINE_KFIFO(mykfifo, char, 64);



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
	int ret;
	if (kfifo_is_empty(&mykfifo)) {
		printk("%s: %s is empty. Going to sleep!\n",__func__, mydevice->name);
		ret = wait_event_interruptible(mydevice->read_queue, !kfifo_is_empty(&mykfifo));
		if (ret)
			return ret;
	}
	int acturl_read;
	ret = kfifo_to_user(&mykfifo, buf, lbuf, &acturl_read);
	if (ret) {
		return -EIO;
	}
	if ( !kfifo_is_full(&mykfifo)) {
		wake_up_interruptible(&mydevice->wirte_queue);
	}
	printk("%s: actual read:%d, pos=%d\n", __func__, acturl_read, *ppos);
	return acturl_read;
}

static ssize_t demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret;
	if (kfifo_is_full(&mykfifo)) {
		printk("%s: %s is full. Going to sleep!\n",__func__, mydevice->name);
		ret = wait_event_interruptible(mydevice->wirte_queue, !kfifo_is_full(&mykfifo));
		if (ret)
			return ret;
	}

	int acturl_write;
	ret = kfifo_from_user(&mykfifo, buf, count, &acturl_write);
	if (ret) 
		return -EIO;
	if (!kfifo_is_empty(&mykfifo)) {
		wake_up_interruptible(&mydevice->read_queue);
	}
	printk("%s: acturl write:%d pos:%d  pid:%d",__func__, acturl_write, *f_pos,current->pid);
	return acturl_write;
}

static const struct file_operations demodrv_fops={
	.owner = THIS_MODULE,
	.open = demodrv_open,
	.release = demodrv_release,
	.read = demodrv_read,
	.write = demodrv_write
};


static int __init simple_char_init(void)
{
	mydevice = kmalloc(sizeof(struct my_device), GFP_KERNEL);
	mydevice->name = DEMO_NAME;
	mydevice->count = 1;
	int ret;
	ret = alloc_chrdev_region(&mydevice->dev, 0 ,mydevice->count ,mydevice->name);	
	if (ret) {
		printk("Failed to allocate char device region!\n");
		return ret;
	}
	mydevice->mycdev = cdev_alloc();
	if (!mydevice->mycdev) {
		printk("Failed to allocate cdev.(cdev_alloc() )\n");
		goto unregister_chrdev;
	}
	
	cdev_init(mydevice->mycdev, &demodrv_fops);
	
	ret = cdev_add(mydevice->mycdev, mydevice->dev,mydevice->count);

	if (ret) {
		printk("cdev_add failed!\n");
		goto cdev_fail;
	}
	
	//init wait queue for read & write
	init_waitqueue_head(&mydevice->read_queue);
	init_waitqueue_head(&mydevice->wirte_queue);

	printk("succeeded register char device : %s, major:%d,minor:%d\n", DEMO_NAME,MAJOR(mydevice->dev),MINOR(mydevice->dev));
	
	return 0;

	cdev_fail:
		cdev_del(mydevice->mycdev);
	unregister_chrdev:
		unregister_chrdev_region(mydevice->dev,mydevice->count);
		
	return ret;
}

static void __exit simple_char_exit(void)
{
	printk("Removing device(%s)\n",DEMO_NAME);
	if (mydevice->mycdev) {
		cdev_del(mydevice->mycdev);
	}
	unregister_chrdev_region(mydevice->dev,mydevice->count);
}



module_init(simple_char_init);
module_exit(simple_char_exit);


MODULE_AUTHOR("Zhong,Ming");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Simple char device");


