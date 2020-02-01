#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kfifo.h>


//static struct kfifo myfifo;

#define DEMO_NAME "my_tmodule"
#define MINOR_COUNT 6
#define MYFIFO_SIZE 64
struct my_device {
	char name[64];
	wait_queue_head_t read_queue;
	wait_queue_head_t wirte_queue;	
	struct kfifo myfifo;
};

struct my_private_date {
	char name[64];
	struct my_device *device;
};

static struct my_device *mydevice[MINOR_COUNT];
static dev_t dev;
static struct cdev *mycdev;




static int demodrv_open(struct inode *inode, struct file *file)
{
	//int major = MAJOR(inode->i_rdev);
	//int minor = MINOR(inode->i_rdev);
	unsigned int minor = iminor(inode);
	unsigned int major = MAJOR(inode->i_rdev);
	printk("%s: major=%d minor=%d\n", __func__, major, minor);
	
	struct my_private_date *date;
	struct my_device *device = mydevice[minor];
	
	date = kmalloc(sizeof(struct my_private_date),GFP_KERNEL);
	date->device = device;
	sprintf(date->name,"private_date_%d",minor);
	
	file->private_data = date;

	return 0;
}

static int demodrv_release(struct inode *inode, struct file *file)
{
	printk("%s enter\n",__func__);
	struct my_private_date *data = file->private_data;
	if(data){
		kfree(data);
		printk("private date free.\n");
	}
	return 0;
}

static ssize_t demodrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t*ppos)
{
	int ret;
	struct my_private_date *data = file->private_data;
	struct my_device *device = data->device;

	if (kfifo_is_empty(&device->myfifo)) {
		printk("%s: %s is empty. Going to sleep!\n",__func__, device->name);
		ret = wait_event_interruptible(device->read_queue, !kfifo_is_empty(&device->myfifo));
		if (ret)
			return ret;
	}
	int acturl_read;
	ret = kfifo_to_user(&device->myfifo, buf, lbuf, &acturl_read);
	if (ret) {
		return -EIO;
	}
	if ( !kfifo_is_full(&device->myfifo)) {
		wake_up_interruptible(&device->wirte_queue);
	}
	printk("%s: actual read:%d, pos=%d\n", __func__, acturl_read, *ppos);
	return acturl_read;
}

static ssize_t demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret;
	struct my_private_date *data = file->private_data;
	struct my_device *device = data->device;
	if (kfifo_is_full(&device->myfifo)) {
		printk("%s: %s is full. Going to sleep!\n",__func__, device->name);
		ret = wait_event_interruptible(device->wirte_queue, !kfifo_is_full(&device->myfifo));
		if (ret)
			return ret;
	}

	int acturl_write;
	ret = kfifo_from_user(&device->myfifo, buf, count, &acturl_write);
	if (ret) 
		return -EIO;
	if (!kfifo_is_empty(&device->myfifo)) {
		wake_up_interruptible(&device->read_queue);
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
	int ret;
	ret = alloc_chrdev_region(&dev, 0 ,MINOR_COUNT ,DEMO_NAME);	
	if (ret) {
		printk("Failed to allocate char device region!\n");
		return ret;
	}
	mycdev = cdev_alloc();
	if (!mycdev) {
		printk("Failed to allocate cdev.(cdev_alloc() )\n");
		goto unregister_chrdev;
	}
	
	printk("cdev alloc succeed!\n");

	cdev_init(mycdev, &demodrv_fops);
	
	printk("cdev init succeed\n");
	ret = cdev_add(mycdev, dev,MINOR_COUNT);

	if (ret) {
		printk("cdev_add failed!\n");
		goto cdev_fail;
	}
	
	int i;
	for (i = 0; i < MINOR_COUNT ;i++){
		mydevice[i] = kmalloc(sizeof(struct my_device), GFP_KERNEL);
		sprintf(mydevice[i]->name,"%s%d",DEMO_NAME,i);
		//init wait queue for read & write
		init_waitqueue_head(&mydevice[i]->read_queue);
		init_waitqueue_head(&mydevice[i]->wirte_queue);
		ret = kfifo_alloc(&mydevice[i]->myfifo,MYFIFO_SIZE,GFP_KERNEL);
		if (ret) {
			ret = -ENOMEM;
			goto free_kfifo;
		}
	}


	printk("succeeded register char device : %s, major:%d,minor:%d\n", DEMO_NAME,MAJOR(dev),MINOR(dev));
	
	return 0;
	
	free_kfifo:
		for (i = 0 ; i < MINOR_COUNT; i++){
			if(&mydevice[i]->myfifo)
				kfree(&mydevice[i]->myfifo);
		}
	free_mydevicd:
		for (i=0; i < MINOR_COUNT; i++){
			if(mydevice[i])
				kfree(mydevice[i]);
		}
	cdev_fail:
		cdev_del(mycdev);
	unregister_chrdev:
		unregister_chrdev_region(dev,MINOR_COUNT);
		
	return ret;
}

static void __exit simple_char_exit(void)
{
	printk("Kfree start(%s)\n",DEMO_NAME);
	int i;
	
	for(i=0; i < MINOR_COUNT; i++) {
		if(mydevice[i]){	
			printk("Kree(mydevice[%d]\n",i);
			kfree(mydevice[i]);
		}
	}

	printk("Removing device(%s)\n",DEMO_NAME);
	if (mycdev) {
		cdev_del(mycdev);
	}
	unregister_chrdev_region(dev,MINOR_COUNT);

	printk("Removing device(%s)\n",DEMO_NAME);
}



module_init(simple_char_init);
module_exit(simple_char_exit);


MODULE_AUTHOR("Zhong,Ming");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Simple char device");


