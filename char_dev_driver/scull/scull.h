#ifndef __SCULL_H
#define __SCULL_H

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include <linux/fs.h>     /* everything... */
#include<linux/cdev.h>

/* input parameters setting*/
//scull device major, use dynamic allocate device 
#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0
#endif 

//scull device minor
#ifndef SCULL_MINOR
#define SCULL_MINOR 0
#endif

//scull device number
#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4    /* scull0 through scull3 */
#endif

//scull device quantum
#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET 1000
#endif

/*
 * Representation of scull quantum sets.
 */
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
	struct scull_qset *data;  /* Pointer to first quantum set */
	int quantum;              /* the current quantum size */
	int qset;                 /* the current array size */
	unsigned long size;       /* amount of data stored here */
	unsigned int access_key;  /* used by sculluid and scullpriv */
	struct semaphore sem;     /* mutual exclusion semaphore     */
	struct cdev cdev;	  /* Char device structure		*/
};


int scull_init_one_module(void);
void scull_cleanup_one_module(void);
int scull_init_more_module(void);
void scull_cleanup_more_module(void);
int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos);
loff_t  scull_llseek(struct file *filp, loff_t off, int whence);

#endif /* __SCULL_H */