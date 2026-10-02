#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "procguard"

static char message[256] = "Process Guardian Driver Active\n";

/* Called when the device is opened */
static int procguard_open(struct inode *inode, struct file *file)
{
    pr_info("procguard: device opened\n");
    return 0;
}

/* Called when the application reads from the device */
static ssize_t procguard_read(struct file *file,
                              char __user *buffer,
                              size_t length,
                              loff_t *offset)
{
    size_t message_length = strlen(message);

    if (*offset >= message_length)
        return 0;

    if (length > message_length - *offset)
        length = message_length - *offset;

    if (copy_to_user(buffer, message + *offset, length))
        return -EFAULT;

    *offset += length;

    return length;
}

/* Called when the application closes the device */
static int procguard_release(struct inode *inode, struct file *file)
{
    pr_info("procguard: device closed\n");
    return 0;
}

/* Device operations */
static const struct file_operations procguard_fops = {
    .owner = THIS_MODULE,
    .open = procguard_open,
    .read = procguard_read,
    .release = procguard_release,
};

/* Device definition */
static struct miscdevice procguard_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &procguard_fops,
};

/* Driver initialization */
static int __init procguard_init(void)
{
    int result;

    result = misc_register(&procguard_device);

    if (result != 0) {
        pr_err("procguard: device registration failed\n");
        return result;
    }

    pr_info("procguard: driver loaded\n");

    return 0;
}

/* Driver cleanup */
static void __exit procguard_exit(void)
{
    misc_deregister(&procguard_device);

    pr_info("procguard: driver unloaded\n");
}

module_init(procguard_init);
module_exit(procguard_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Linux Process Guardian Character Device Driver");
MODULE_VERSION("1.0");
