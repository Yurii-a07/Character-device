#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define DEV_MEM_SIZE 512

uint8_t* device_buffer;
dev_t device_number;
struct cdev ccdev;
static struct class* ccdev_class;
struct file_operations fops_ccdev;

static int ccdev_open (struct inode* inod, struct file* fil){
	printk(KERN_INFO "Device opened...:)\n");
	device_buffer = kmalloc(DEV_MEM_SIZE, GFP_KERNEL);
	if(!device_buffer){
		printk(KERN_INFO "Can not allocate kernel memory...\n");
		return -1;
	}
	return 0;
}

static int ccdev_release(struct inode* inod, struct file* fil){
	kfree(device_buffer);
	printk(KERN_INFO "Device FILE closed...\n");
	return 0;
}

static ssize_t ccdev_read(struct file *fil, char __user *buf, size_t len, loff_t* off){
	copy_to_user(buf, device_buffer, DEV_MEM_SIZE);
	printk(KERN_INFO "Buffer copied to user...\n"); 
	return DEV_MEM_SIZE;
}

static ssize_t ccdev_write(struct file* fil, const char* buff, size_t size, loff_t* off){
	copy_from_user(device_buffer, buff, DEV_MEM_SIZE);
	printk(KERN_INFO "Buffer copied from user...");
	return DEV_MEM_SIZE;
}





struct file_operations fops_ccdev = {
	.read = ccdev_read,
	.write = ccdev_write,
	.open = ccdev_open,
	.owner = THIS_MODULE,
	.release = ccdev_release
};



static int __init ccdev_init(void){

	printk(KERN_INFO "ccdev initialization...\n");

	if((alloc_chrdev_region(&device_number,0,1,"chrdev")) < 0){
		printk(KERN_INFO "Can not allocate major number\n");
		return -1;
	}

	printk(KERN_INFO "Major %d, Minor %d...\n", MAJOR(device_number), MINOR(device_number));

	cdev_init(&ccdev, &fops_ccdev);

	ccdev.owner = THIS_MODULE;
	if(cdev_add(&ccdev, device_number, 1) < 0){
		printk(KERN_INFO "Can not add device to the system...\n");
		goto ur_class;
	}

	if((ccdev_class = class_create(THIS_MODULE, "ccdev_class")) == NULL){
		printk(KERN_INFO "Can not create ccdev_class...\n");
		goto ur_class;
	}

	if(device_create(ccdev_class, NULL, device_number, NULL, "ccdev") == NULL){
		printk(KERN_INFO "Can not create the device device...\n");
		goto ur_device;
	}

	printk(KERN_INFO "Device driver insert...done properly...\n");

	return 0;

ur_device:
	class_destroy(ccdev_class);

ur_class:
	unregister_chrdev_region(device_number, 1);
	return -1;
}
module_init(ccdev_init);

static void __exit ccdev_exit(void){
	device_destroy(ccdev_class, device_number);
	class_destroy(ccdev_class);
	cdev_del(&ccdev);
	unregister_chrdev_region(device_number,1);
	printk(KERN_INFO "ccdev removed.\n");
}
module_exit(ccdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("me");
MODULE_DESCRIPTION("Charecter device");
