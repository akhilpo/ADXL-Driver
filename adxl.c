/* 	Warning: This is a very basic device driver. There are lot of best practices which should be followed for efficiency and safety
	which I have avoided here for simplicity and due to time constrain. Use this at your own risk.
	And read the README file before checking out the source code.
	Akhil.
*/



#include <linux/kernel.h>      
#include <linux/module.h>      
#include <linux/fs.h>
#include <asm/uaccess.h>       
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/gpio.h>                 // Required for the GPIO functions
#include <linux/interrupt.h> 
#include <linux/workqueue.h>

#define DEVICE_NAME "adxl"
#define MAJOR_NO 115

//Register definitions
#define REG_DEVID 0x00
#define REG_DATA_FORMAT 0x31
#define REG_INT_ENABLE 0x2E
#define REG_POWER_CTL 0x2D
#define REG_INT_MAP 0x2F
#define REG_INT_SOURCE 0x30
#define REG_DATAX0 0x32
#define REG_DATAX1 0x33
#define REG_DATAY0 0x34
#define REG_DATAY1 0x35
#define REG_DATAZ0 0x36
#define REG_DATAZ1 0x37

#define INT_GPIO 60 

static struct adxl_data_struct {
	u8 xdata;
	u8 ydata;
	u8 zdata;
	unsigned int irq;
} adxl_data;

static struct class *adxl_class;
static dev_t dev_number;
struct class *cl;
static struct adxl_data data;
static unsigned int irqNumber;
static struct i2c_client * my_client;
struct i2c_adapter * my_adap;
static dev_t first;
static struct cdev c_dev;
static u8 axis_data[3];

static void mykmod_work_handler(struct work_struct *w);

static struct workqueue_struct *wq = 0;
static DECLARE_WORK(mykmod_work, mykmod_work_handler);


static u8 adxl_read(struct i2c_client *client, u8 reg)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0)
		dev_err(&client->dev,
			"can not read register, returned %d\n", ret);

	return (u8)ret;
}

static int adxl_write(struct i2c_client *client, u8 reg, u8 data)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, data);
	if (ret < 0)
		dev_err(&client->dev,"can not write register, returned %d\n", ret);

	return ret;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {	
//	u8 readvalue[4];
//	readvalue[3] = NULL;
	printk(KERN_INFO "Driver read()\n");
/*	
	readvalue[0] = adxl_read(my_client, REG_DATAX1); 
	readvalue[1] = adxl_read(my_client, REG_DATAY1); 
	readvalue[2] = adxl_read(my_client, REG_DATAZ1); 
*/
//	printk(KERN_INFO "readvalue = %d, off = %d",readvalue,*off);
//	readvalue = 'a';
	if (*off == 0)
	{
		if (copy_to_user(buf, &axis_data,3) != 0)
		{
			printk(KERN_INFO "Driver read: Inside if\n");
			return -EFAULT;
		}
		else 
		{
		//	readvalue = 0;
		//	copy_to_user(buf+1, &readvalue, 1);
	//		(*off)++;
		//	(*off)++;
		//	len = 1;
			return 3;
		}
	}
	else
		return 0;
}


static int my_open(struct inode *i, struct file *f)
{
	  printk(KERN_INFO "Driver: open()\n");
	    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	  printk(KERN_INFO "Driver: close()\n");
	    return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf,
		   size_t len, loff_t *off)
{
	  printk(KERN_INFO "Driver: write()\n");
	    return len;
}

static struct file_operations my_dev_ops = {.read = my_read,
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.write = my_write};

static irq_handler_t adxl_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) 
{
	static unsigned int intCount = 0;
	intCount++;
	printk(KERN_INFO "ADXL: interrupt_handler(),Count=%d\n",intCount);
/*	
	axis_data[0] = adxl_read(my_client, REG_DATAX1); 
	axis_data[1] = adxl_read(my_client, REG_DATAY1); 
	axis_data[2] = adxl_read(my_client, REG_DATAZ1);
*/
	if (wq)
		queue_work(wq, &mykmod_work);

	return (irq_handler_t) IRQ_HANDLED;
}

static void mykmod_work_handler(struct work_struct *w)
{
	printk(KERN_INFO "ADXL: inside mykmod_work_handler\n");

	axis_data[0] = adxl_read(my_client, REG_DATAX1); 
	axis_data[1] = adxl_read(my_client, REG_DATAY1); 
	axis_data[2] = adxl_read(my_client, REG_DATAZ1);
}

static int __init adxl_init(void)
{
//	struct adxl_data *data;
	int ret_val,errno;
	int result = 0;
	u8 readvalue,temp; 
        printk(KERN_INFO "Welcome to ADXL345 Accelerometer!\n");
       
        if (alloc_chrdev_region(&first, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_DEBUG "Can't register device\n");
        return -1;
        }
//        ret_val = register_chrdev(MAJOR_NO, DEVICE_NAME, &my_dev_ops);
	if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv"))){
			unregister_chrdev_region(first, 1);
			}

	if (IS_ERR(device_create(cl, NULL, first, NULL, "adxl"))) {
		class_destroy(cl);
		unregister_chrdev_region(first,1);
	}

	cdev_init(&c_dev,&my_dev_ops);
	if (cdev_add(&c_dev, first,1) < 0)
	{
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first,1);
	return -1;
	}

      /*  if (ret_val < 0) {
        printk(KERN_ALERT "%s failed with %d\n",
        "Sorry, registering the character device ", ret_val);
        return ret_val;
}
 */

        //device_create(adxl_class, NULL,dev_number, NULL, DEVICE_NAME);
 
       /* ret_val = 0; //i2c_add_driver(&adxl_driver);
        if (ret_val) {
        printk("Registering I2C driver failed, errno is %d\n", ret_val);
        return ret_val;
        }
*/
	my_adap = i2c_get_adapter(2); // 1 means i2c-1 bus
	if (!(my_client = i2c_new_dummy(my_adap, 0x53))){
		printk(KERN_INFO "Couldn't acquire i2c slave");
		unregister_chrdev(MAJOR_NO, DEVICE_NAME);
		device_destroy(cl, first);
		class_destroy(cl);
		return -1;
	}
		
	readvalue = adxl_read(my_client,0x00);
	if (readvalue == (0b11100101)) {
		printk("Accelerometer detected, value = %d\r\n",readvalue);
	}

	//Setting Data format to be left justified
	temp = adxl_read(my_client,REG_DATA_FORMAT);
	temp = temp | (1<<2);
	adxl_write(my_client, REG_DATA_FORMAT, temp);
	
	//Enabling the interrupt
	
/*	temp = adxl_read(my_client,REG_INT_MAP);
	temp = temp | (1<<7);
	adxl_write(my_client, REG_INT_MAP, temp);
*/	
	temp = adxl_read(my_client,REG_INT_ENABLE);
	temp = temp | (1<<7);
	adxl_write(my_client, REG_INT_ENABLE, temp);

	
	
	//Start measurement
	temp = adxl_read(my_client,REG_POWER_CTL);
	temp = temp | (1<<3);
	adxl_write(my_client, REG_POWER_CTL, temp);

	//Interrupt config
/*	if((errno = gpio_direction_input(GPIO)) !=0)
	{
		printk(KERN_INFO "Can't set GPIO direction, error %i\n", errno);
		gpio_free(GPIO);
		return -EINVAL;
	}*/
	if (!wq)
		wq = create_singlethread_workqueue("mykmod");

	irqNumber = gpio_to_irq(INT_GPIO);
	result = request_irq(irqNumber, (irq_handler_t) adxl_irq_handler,
			IRQF_TRIGGER_RISING,
			"ADXL_INT",
			NULL);
	printk(KERN_INFO "ADXL: The interrupt request result is: %d\n", result);

	axis_data[0] = adxl_read(my_client, REG_DATAX1);
	axis_data[1] = adxl_read(my_client, REG_DATAY1);
	axis_data[2] = adxl_read(my_client, REG_DATAZ1);

	return 0;
}
 

static void __exit adxl_exit(void)
{
        printk(KERN_INFO "ADXL: Removing moduleq");
 	
	free_irq(irqNumber,NULL);
	destroy_workqueue(wq);
	i2c_unregister_device(my_client);
        cdev_del(&c_dev);
//	unregister_chrdev(MAJOR_NO, DEVICE_NAME);
 	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
        //i2c_del_driver(&imu_driver);
 
}


module_init(adxl_init);
module_exit(adxl_exit);

MODULE_DESCRIPTION("driver");
MODULE_AUTHOR("Akhil");
MODULE_LICENSE("GPL");
