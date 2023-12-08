// my first scull driver
#include "scull.h"
#include <asm/uaccess.h> /* copy_*_user */
#include <linux/cdev.h>
#include <linux/fs.h> /* everything... */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/uaccess.h>

// LICENSE
MODULE_LICENSE("GPL");
// Author
MODULE_AUTHOR("SX_CONAN");
// driver version
MODULE_VERSION("char_dev_v2");

int scull_major = SCULL_MAJOR;
int scull_minor = SCULL_MINOR;
struct cdev cdev;                 // char device structor
static struct class *scull_class; // scull设备的逻辑类

int scull_open(struct inode *inode, struct file *filp) {
  pr_info( "conan:Enter scull open\n");
  return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
  pr_info( "conan: Enter scull release\n");
  return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) 
{
  pr_info( "conan : Enter scull read\n");
  return 0;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos) 
{
  pr_info( "conan : Enter scull write\n");
  return 0;
}

struct file_operations scull_fops = {
    .owner = THIS_MODULE,
    .read = scull_read,
    .write = scull_write,
    .open = scull_open,
    .release = scull_release,
};

/*
 * Set up the char_dev structure for this device.
 */
static long scull_setup_cdev(struct cdev cdev, int index) {
  struct device *scull_device;
  dev_t dev;
  int err, devno = MKDEV(scull_major, scull_minor + index);
  dev = MKDEV(scull_major, scull_minor);

  scull_class = class_create(THIS_MODULE, "scull_class"); // 动态创建scull设备的逻辑类
  if (IS_ERR(scull_class)) {
    pr_err("Error creating scull char class.\n");
    unregister_chrdev_region(MKDEV(scull_major, 0), 1);
    return PTR_ERR(scull_class);
  }

  cdev_init(&cdev, &scull_fops);
  cdev.owner = THIS_MODULE;
  cdev.ops = &scull_fops;
  err = cdev_add(&cdev, devno, 1);
  /* Fail gracefully if need be */
  if (err)
    printk(KERN_ALERT "conan:Error %d adding scull%d", err, index);
   // printk(KERN_NOTICE "Error %d adding scull%d", err, index);

  //使用device_create函数在/dev目录下创建scull_char设备
  scull_device = device_create(scull_class, NULL, /* no parent device */
                               dev,               /* associated dev_t */
                               NULL,              /* no additional data */
                               "scull_char");     /* device name */

  if (IS_ERR(scull_device)) {
    pr_err("Error creating scull char device.\n");
    class_destroy(scull_class);
    unregister_chrdev_region(dev, 1);
    return -1;
  }


  return 0;
}

static int scull_init_module(void) {
  int result;
  dev_t dev = 0;
  // 动态申请设备号，设备名称"scull",设备个数1，次设备号scull_minor,申请到的设备号存储在dev中。该函数返回值小于0表示申请失败。
  result = alloc_chrdev_region(&dev, scull_minor, 1,"scull"); 
  scull_major = MAJOR(dev);
  if (result < 0) {
    printk(KERN_WARNING "scull:?can't?get?major?%d\n", scull_major);
    return result;
  }
  scull_setup_cdev(cdev, 0);
  pr_alert("conan:scull char module loaded\n");

  return 0;
}

static void scull_exit_module(void) {
  /* cleanup_module is never called if registering failed */
  dev_t dev;
  dev = MKDEV(scull_major, scull_minor);
  unregister_chrdev_region(dev, 1);
  device_destroy(scull_class, MKDEV(scull_major, 0));
  cdev_del(&cdev);
  class_destroy(scull_class);

  pr_alert("conan:scull char module Unloaded\n");
}

module_init(scull_init_module);
module_exit(scull_exit_module);
