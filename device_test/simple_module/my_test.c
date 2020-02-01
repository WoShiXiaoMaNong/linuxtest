#include <linux/init.h>
#include <linux/module.h>

static int debug = 1;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug,"Enable debugging informatoin");

#define dprintk(args...) \
	if(debug) { \
		printk(KERN_DEBUG args); \
	}

static int mytest = 100;
module_param(mytest, int, 0644);
MODULE_PARM_DESC(mytest,"test for module paramter");



static int __init my_test_init(void)
{
	dprintk("My module test and Hello World\n");
	dprintk("module paramter=%d\n",mytest);
	return 0;
}


static void __exit my_test_exit(void)
{
	printk("My module exit");
}

module_init(my_test_init);
module_exit(my_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhong,Ming");
MODULE_DESCRIPTION("My Kernel Moudle Paramters test");
MODULE_ALIAS("mytest");
