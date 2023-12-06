#ifndef __CHAR_DEV_H
#define __CHAR_DEV_H

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include <linux/fs.h>     /* everything... */

int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                   loff_t *f_pos);
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos);
loff_t  scull_llseek(struct file *filp, loff_t off, int whence);

#endif /* __CHAR_DEV_H */