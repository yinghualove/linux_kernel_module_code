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
MODULE_VERSION("char_dev_v1");

// define scull device parameters
int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS; /* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;

// get scull device parameters from input
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

struct cdev cdev; /* Char device structure */
struct scull_dev *scull_devices;	/* allocated in scull_init_module */

#define MAX_SIZE 10
size_t size = 0;
char store[MAX_SIZE];

/*
 * Empty out the scull device; must be called with the device
 * semaphore held.
 * sucll_trim 函数负责释放整个数据区, 由 scull_open 在文件为写而打开时调
 * 用. 它简单地遍历列表并且释放它发现的任何量子和量子集.
 */
int scull_trim(struct scull_dev *dev) {
  struct scull_qset *next, *dptr;
  int qset = dev->qset; /* "dev" is not-null */
  int i;

  for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
    if (dptr->data) {
      for (i = 0; i < qset; i++)
        kfree(dptr->data[i]);
      kfree(dptr->data);
      dptr->data = NULL;
    }
    next = dptr->next;
    kfree(dptr);
  }
  dev->size = 0;
  dev->quantum = scull_quantum;
  dev->qset = scull_qset;
  dev->data = NULL;
  return 0;
}

/*
 * Open and close
 */
int scull_open(struct inode *inode, struct file *filp) {
  /* trim to 0 the length of the device if open was write -only
   */
  if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
    size = 0;
  }
  return 0; /* success */
}

int scull_release(struct inode *inode, struct file *filp) { return 0; }

/*
 * Data management: read and write
 */
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos) {
  ssize_t retval = 0;
  if (*f_pos >= size)
    goto out;
  if (*f_pos + count > size)
    count = size - *f_pos;
  if (copy_to_user(buf, store + *f_pos, count)) {
    retval = -EFAULT;
    goto out;
  }
  *f_pos += count;
  retval = count;
out:
  return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos) {
  ssize_t retval = -ENOMEM; /* value used in "goto out"statements */
  if (*f_pos >= MAX_SIZE)
    goto out;
  if (*f_pos + count > MAX_SIZE)
    count = MAX_SIZE - *f_pos;
  if (copy_from_user(store + *f_pos, buf, count)) {
    retval = -EFAULT;
    goto out;
  }
  *f_pos += count;
  retval = count;
  /* update the size */
  if (size < *f_pos)
    size = *f_pos;
out:
  return retval;
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
static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err, devno = MKDEV(scull_major, scull_minor + index);
    
	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

int scull_init_more_module(void) {
  int result, i;
  // 在linux中dev_t是unsigned int类型，32位，用于在驱动程序中定义设备编号，高12位为主设备号，低20位为次设备号
  dev_t dev =   0; 
 
/*
 * Get a range of minor numbers to work with, asking for a dynamic
 * major unless directed otherwise at load time.
 */
	if (scull_major) {
		//主设备号已知，使用register_chrdev_region函数注册字符设备
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {
		//主设备号未知，有alloc_chrdev_region函数动态分配主设备号和次设备号
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
				"scull");
		scull_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return result;
	}
        /* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}
	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

        /* Initialize each device. */
	for (i = 0; i < scull_nr_devs; i++) {
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
   		sema_init(&scull_devices[i].sem, 1);	
		scull_setup_cdev(&scull_devices[i], i);
	}

	return 0; /* succeed */

 fail:
	scull_cleanup_more_module();
	return result;

}


void scull_cleanup_more_module(void) {
  int i;
  dev_t devno = MKDEV(scull_major, scull_minor);

  /* Get rid of our char dev entries */
  if (scull_devices) {
    for (i = 0; i < scull_nr_devs; i++) {
      scull_trim(scull_devices + i);
      cdev_del(&scull_devices[i].cdev);
    }
    kfree(scull_devices);
  }

#ifdef SCULL_DEBUG /* use proc only if debugging */
  scull_remove_proc();
#endif

  /* cleanup_module is never called if registering failed */
  unregister_chrdev_region(devno, scull_nr_devs);

  /* and call the cleanup functions for friend devices */
//   scull_p_cleanup();
//   scull_access_cleanup();
}


module_init(scull_init_more_module);
module_exit(scull_cleanup_more_module);
