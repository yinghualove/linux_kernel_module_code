#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/slab.h>   /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/uaccess.h> /* copy_*_user */
#include "char_dev.h"

/*
 * Open and close
 */



int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

struct file_operations scull_fops = {
        .owner = THIS_MODULE ,
        .llseek = scull_llseek,
        .read = scull_read ,
        .write = scull_write ,
        // .iopoll = scull_iopull,
        .open = scull_open ,
        .release = scull_release ,
};

int scull_major = 3;  // scull主设备号
int scull_minor = 23; // scull次设备号
struct cdev cdev;

static int scull_init_module(void)
{
    printk(KERN_ALERT "start to create a char dev: name scull\n");
    int result;
    dev_t dev = 0;                                              // 在linux中是unsigned int 类型，32位，用于在驱动程序中定义设备编号，高12位为主设备号，低20位为次设备号
    result = alloc_chrdev_region(&dev, scull_minor, 1, "name"); // 动态创建字符设备
    scull_major = MAJOR(dev);

	//通过 cdev_init( ) 建立cdev与 file_operations之间的连接，通过 cdev_add( ) 向系统添加一个cdev以完成注册;
	cdev_init(&cdev,&scull_fops);//用上面声明的scull_fops初始化cdev
	cdev.owner = THIS_MODULE;
	cdev.ops = &scull_fops;
	result=cdev_add(&cdev,dev,1);  //创建设备；
	if(result){
		printk("error\n");
		unregister_chrdev_region(dev,1);
		return result;
	}

    return 0;
}

static void scull_exit_module(void)
{
    dev_t dev = 0;                                              
    printk(KERN_ALERT "Gooble ,cruel world\n");
	unregister_chrdev_region(dev,1);
}

module_init(scull_init_module);
module_exit(scull_exit_module);

// License
MODULE_LICENSE("Dual BSD/GPL");
// Author
MODULE_AUTHOR("scull");
// Version
MODULE_VERSION("scull_V1");