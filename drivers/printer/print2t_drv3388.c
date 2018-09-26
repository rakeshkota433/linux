/***********************************************************/
// This Driver written for Thermal 2inch printer driver  
//
/***********************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/clk.h>
#include <linux/errno.h>
#include <linux/irq.h>
#include <linux/list.h>
//#include <mach/hardware.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include <asm/pmu.h>
#include "Am335x_gpio.h"

/************************************************************************************/

/************************************************************************************/
//#define IMAGE_SIZE 4194304
#define NULL_TERM  '\0'

#define ON         1
#define OFF        0

#define F_HALF     2
#define S_HALF     3
#define _FULL_     4

#define RF_HALF    5
#define RS_HALF    6
#define R_FULL_    7

#define STEP1	   1
#define STEP2      2
#define STEP3      3
#define STEP4      4

//PRINTER STROBES ADDED BY RAKESH ANALOGICS
/*#define STB1   	GPIO_TO_PIN(3, 21) //MX6UL_PAD_CSI_DATA00__GPIO4_IO21
#define STB2   	GPIO_TO_PIN(3, 22) //MX6UL_PAD_CSI_DATA01__GPIO4_IO22
#define STB3   	GPIO_TO_PIN(3, 23) //MX6UL_PAD_CSI_DATA02__GPIO4_IO23
#define STB4   	GPIO_TO_PIN(3, 24) //MX6UL_PAD_CSI_DATA03__GPIO4_IO24
#define STB5   	GPIO_TO_PIN(3, 26) //MX6UL_PAD_CSI_DATA05__GPIO4_IO26
#define STB6   	GPIO_TO_PIN(3, 13) //MX6UL_PAD_NAND_CE0_B__GPIO4_IO13 

//STEPPER MOTOR FOR DRV8833 BY RAKESH ANALOGICS
#define ENA1     GPIO_TO_PIN(0, 26) //MX6UL_PAD_UART3_CTS_B__GPIO1_IO26
#define ENA2     GPIO_TO_PIN(1, 8) //MX6UL_PAD_ENET2_RX_DATA0__GPIO2_IO08
#define IN1      GPIO_TO_PIN(1, 9) //MX6UL_PAD_ENET2_RX_DATA1__GPIO2_IO09
#define IN2      GPIO_TO_PIN(0, 27) //MX6UL_PAD_UART3_RTS_B__GPIO1_IO27*/

//PRINTER CONTROL PINS ADDED BY RAKESH ANALOGICS
#define PRN_PW  GPIO_TO_PIN(3,  20) //MX6UL_PAD_CSI_HSYNC__GPIO4_IO20
#define LAT  	GPIO_TO_PIN(3,  14) //MX6UL_PAD_NAND_CE1_B__GPIO4_IO14

#define Set(X,V) gpio_set_value(X,V)

#define CS_CHANGE(Pxfer)        ((Pxfer).cs_change = 0)

#define Pwidth  48
//char print_buf[IMAGE_SIZE];

//###################################analogic new
#include <linux/timer.h>
//static struct timer_list printer_timer;
//static unsigned int BHeight1;
extern int epit_timer(int );
void Latch(void);
extern void epit_off(void);

static int count;
static int lock;
struct node_2
{
    unsigned char *data;
    int BUF_LEN;
    struct node *ptr;
}*front,*rear,*temp,*front1,*front2;

void create(void)
{
    front = rear = NULL;
}
 
 
/* Dequeing the queue */
void deq(void)
{
    front1 = front;
 
    if (front1 == NULL)
    {
        printk("\n Error: Trying to display elements from empty queue");
        return;
    }
    else
        if (front1->ptr != NULL)
        {
            front1 = front1->ptr;
           //printk("\n Dequed value front1");
	    kfree(front->data);
            kfree(front);
            front = front1;
        }
        else
        {
           //printk("\n Dequed value front");
	    kfree(front->data);
            kfree(front);
            front = NULL;
            rear = NULL;
        }
        count--;
}
 



//void my_timer_callback( unsigned long data );
//#####################################

/****************************/
/****************************************************************************************/
/*											*/
/****************************************************************************************/

//This for ADC RAW RED FUNCTION WICH IS EXPORTED THERE TO USE HERE
//static int tiadc_read_raw(struct iio_dev,struct iio_chan_spec const *,int *,int *,long);
//int tiadc_read_raw(struct iio_dev,struct iio_chan_spec const *,int *,int *,long);

/****************************************************************************************/
		
const unsigned char bar[]={0x06,0xCC,0x06,0x6C,0x06,0x66,0x04,0x98,0x04,0x8C,
	0x04,0x4C,0x04,0xC8,0x04,0xC4,0x04,0x64,0x06,0x48,0x06,0x44,0x06,0x24,0x05,
	0x9C,0x04,0xDC,0x04,0xCE,0x05,0xCC,0x04,0xEC,0x04,0xE6,0x06,0x72,0x06,0x5C,
	0x06,0x4E,0x06,0xE4,0x06,0x74,0x07,0x6E,0x07,0x4C,0x07,0x2C,0x07,0x26,0x07,
	0x64,0x07,0x34,0x07,0x32,0x06,0xD8,0x06,0xC6,0x06,0x36,0x05,0x18,0x04,0x58,
	0x04,0x46,0x05,0x88,0x04,0x68,0x04,0x62,0x06,0x88,0x06,0x28,0x06,0x22,0x05,
	0xB8,0x05,0x8E,0x04,0x6E,0x05,0xD8,0x05,0xC6,0x04,0x76,0x07,0x76,0x06,0x8E,
	0x06,0x2E,0x06,0xE8,0x06,0xE2,0x06,0xEE,0x07,0x58,0x07,0x46,0x07,0x16,0x07,
	0x06,0x4E,0x06,0xE4,0x06,0x74,0x07,0x6E,0x07,0x4C,0x07,0x2C,0x07,0x26,0x07,
	0x64,0x07,0x34,0x07,0x32,0x06,0xD8,0x06,0xC6,0x06,0x36,0x05,0x18,0x04,0x58,
	0x04,0x46,0x05,0x88,0x04,0x68,0x04,0x62,0x06,0x88,0x06,0x28,0x06,0x22,0x05,
	0xB8,0x05,0x8E,0x04,0x6E,0x05,0xD8,0x05,0xC6,0x04,0x76,0x07,0x76,0x06,0x8E,
	0x06,0x2E,0x06,0xE8,0x06,0xE2,0x06,0xEE,0x07,0x58,0x07,0x46,0x07,0x16,0x07,
	0x68,0x07,0x62,0x07,0x1A,0x07,0x7A,0x06,0x42,0x07,0x8A,0x05,0x30,0x05,0x0C,
	0x04,0xB0,0x04,0x86,0x04,0x2C,0x04,0x26,0x05,0x90,0x05,0x84,0x04,0xD0,0x04,
	0xC2,0x04,0x34,0x04,0x32,0x06,0x12,0x06,0x50,0x07,0xBA,0x06,0x14,0x04,0x7A,
	0x05,0x3C,0x04,0xBC,0x04,0x9E,0x05,0xE4,0x04,0xF4,0x04,0xF2,0x07,0xA4,0x07,
	0x94,0x07,0x92,0x06,0xDE,0x06,0xF6,0x07,0xB6,0x05,0x78,0x05,0x1E,0x04,0x5E,
	0x05,0xE8,0x05,0xE2,0x07,0xA8,0x07,0xA2,0x05,0xDE,0x05,0xEE,0x07,0x5E,0x07,
	0xAE};

//char print_buf[IMAGE_SIZE];
int lCnt=0;
int Buf1Cnt;
unsigned char mot_flag;

struct prn_bus{
        struct spi_message    Pmsg;
        struct spi_transfer   Pxfer;
};

struct spi_device pl,*pspi;

//static void Format_spi_data(struct device *dev,unsigned char *print_buf)
void Format_spi_data(struct device *dev)
{
	int status,i;
	front2 = front;	
	static int line_count;
	struct spi_device *spi =  to_spi_device(dev);
  	struct prn_bus    *bus =  kzalloc(sizeof *bus,GFP_KERNEL);
       
  

	spi_message_init(&bus->Pmsg);
//	bus->Pxfer.tx_buf = &print_buf[lCnt];
//	for(i=0;i<48;i++)
//	printk(" %x  ",front2->data[i]);
        //printk("\n");
	bus->Pxfer.tx_buf = &front2->data[lCnt];
	bus->Pxfer.len = Pwidth;
	CS_CHANGE(bus->Pxfer);
	spi_message_add_tail(&bus->Pxfer,&bus->Pmsg);
	//disable_irq(&bus->spi->irq);
//	status = spi_async(spi,&bus->Pmsg);
	spi_sync_locked(spi,&bus->Pmsg);
//	printk("Status=%d\n",status);
	//enable_irq(&bus->spi->irq);
	kfree(bus);

	lCnt+=Pwidth;

//	Set(LAT,0);
//	Set(LAT,1);
}
void EPIT_SPI_SEND(void)
{
  //printk("EPIT_SPI_SEND");
  Format_spi_data(&pspi->dev);
	
}
EXPORT_SYMBOL(EPIT_SPI_SEND);

static void Format_spi_data_char(struct device *dev,char *dot_buf)
{
        int status;
        struct spi_device *spi =  to_spi_device(dev);
        struct prn_bus    *bus =  kzalloc(sizeof *bus,GFP_KERNEL);



        spi_message_init(&bus->Pmsg);
        bus->Pxfer.tx_buf = &dot_buf[lCnt];
        bus->Pxfer.len = Pwidth;
        CS_CHANGE(bus->Pxfer);
        spi_message_add_tail(&bus->Pxfer,&bus->Pmsg);
        //disable_irq(&bus->spi->irq);
        status = spi_sync(spi,&bus->Pmsg);
        //enable_irq(&bus->spi->irq);
        kfree(bus);

        //lCnt+=Pwidth;

      Set(LAT,0);
      udelay(50); 
      Set(LAT,1);


}

void PrinterPower(unsigned char State)
{
	if(count==1){
		epit_off();
		Set(PRN_PW, 0);
		deq();
		deq();
		lCnt=0;
		lock=0;
		//epit_off();
	}
       	else{
		deq();
		lCnt=0;
                //front = front1->ptr;
		//printk("\nfront->BUF_LEN_count=%d \n",front->BUF_LEN);
		EPIT_SPI_SEND();
		Latch();
		epit_timer(front->BUF_LEN);

	}



}
EXPORT_SYMBOL_GPL(PrinterPower);
void Latch(void)
{
	Set(LAT,0);
      	Set(LAT,1);
}
EXPORT_SYMBOL_GPL(Latch);

void PrinterInit(void)
{
   int Ret=9;
     create(); 
        Ret=Gpio_set_value(PRN_PW,"VH",1,0);
//	printk("PRN_PW Ret=%d\n",Ret);
        Ret=Gpio_set_value(LAT,"LATCH",1,0);
//	printk("LAT Ret=%d\n",Ret);
	

}

/*********************************************************/
// FUJITSU - FTP-628MCL103 - Motor Sequnce               //
/*********************************************************/
//  FORWARD SEQUENCE FOR MOTOR PINS
//        
//         ENA1 ENA2 IN1 IN2  	A   B   A!  B!
// Step1    0	 1    1   0  ->	0   0   1   1  ] 
// Step2    0    1    0   1  ->	0   1   1   0  ]` 1ST Half Cycle 
//
// Step3    1    0    0   1  ->	1   1   0   0  ] 
// Step4    1    0    1   0  -> 1   0   0   1  ]` 2ND Half Cycle
//
/********************************************************/

/*********************************************************/
// FUJITU - FTP-638MCL101 - 8 step Motor Sequnce        //
/*********************************************************/
//  FORWARD SEQUENCE FOR MOTOR PINS
//        
//         ENA1 ENA2 IN1 IN2    A   B   A!  B!
// Step1    1    0    1   0  -> 1   0   0   1  ]   
// Step2    1    1    1   0  -> 0   0   0   1  ]     
// Step3    0    1    1   0  -> 0   0   1   1  ]` 1ST Half Cycle
// Step4    0    1    1   1  -> 0   0   1   0  ]    
//
// Step5    0    0    0   1  -> 0   1   1   0  ]    
// Step6    1    1    0   1  -> 0   1   0   0  ]   
// Step7    1    0    0   1  -> 1   1   0   0  ]`2ND Half Cycle
// Step8    1    0    1   1  -> 1   0   0   0  ]   
/********************************************************/


/*************************************************************************/
// FUJITSU - FTP-638MCL101 - 8 step Motor Sequnce for drv8833 motor driver//
/*************************************************************************/
//  FORWARD SEQUENCE FOR MOTOR PINS FOR DRV8833 BY AMARENDAR D ON 04022015
//        
//         ENA1 ENA2 IN2  IN1     A   A!  B  B!
//         AIN1 AIN2 BIN1 BIN2
// Step1    1    0    0    1  ->  1   0   0   1  ]   
// Step2    1    1    0    1  ->  0   0   0   1  ]     
// Step3    0    1    0    1  ->  0   1   0   1  ]` 1ST Half Cycle
// Step4    0    1    1    1  ->  0   1   0   0  ]    
//
// Step5    0    0    1    0  ->  0   1   1   0  ]    
// Step6    1    1    1    0  ->  0   0   1   0  ]   
// Step7    1    0    1    0  ->  1   0   1   0  ]`2ND Half Cycle
// Step8    1    0    1    1  ->  1   0   0   0  ]   
/********************************************************/






static int user_prn_read(struct file *file, char *buf, unsigned char count, loff_t *ppos)
{
	//struct iio_dev adcdev;
	//struct iio_chan_spec const chan;
        int val,val2;
	//long mask;

        //wait_event_interruptible(prin_queue,wevent!=0);
        copy_to_user(buf,"001",3);

	//tiadc_read_raw(adcdev,&chan,&val,&val2,mask);
        //while(1)
        {
                PrinterPower(ON);
                //Cycle(_FULL_,1250);
                PrinterPower(OFF);
        }
	printk("\n READ SUCCESS  Valu=%d\n",val);

        return 0;
}

//###################################analogic new
//#####################################


  

//static int user_prn_write(struct file * file,unsigned char *print_buf,unsigned int BHeight, loff_t *loft) {
static int user_prn_write(struct file * file,unsigned char *buf,unsigned int BHeight, loff_t *loft) {

	int Dlcnt=0; 
//	static int count;
//	enq(&buf,BHeight);
	//	copy_from_user(print_buf,buf, BHeight*Pwidth );
	{
		int i ;
		if (rear == NULL)
		{
			rear = (struct node *)kmalloc(1*sizeof(struct node),GFP_KERNEL);
			rear->ptr = NULL;
			rear->BUF_LEN=BHeight;
			rear->data = (unsigned char *)kmalloc(BHeight*Pwidth*sizeof(unsigned char) , GFP_KERNEL);
			copy_from_user(rear->data,buf, BHeight*Pwidth );
			//printk("\nrear==null");
	//		for(i=0;i<BHeight*Pwidth;i++)
	//			printk("%x",rear->data[i]);	
			front = rear;
		}
		else
		{
			temp=(struct node *)kmalloc(1*sizeof(struct node),GFP_KERNEL);
			rear->ptr = temp;
			//printk("\nrear==tmp");
			temp->BUF_LEN=BHeight;
			temp->data =  (unsigned char *)kmalloc(BHeight*Pwidth*sizeof(unsigned char), GFP_KERNEL);
			copy_from_user(temp->data,buf, BHeight*Pwidth );
		//	for(i=0;i<BHeight*Pwidth;i++)
                  //          printk("%x",rear->data[i]);
			temp->ptr = NULL;

			rear = temp;
		}
		count++;
	}

	//lCnt=0;
	Dlcnt=0; 
	//PrinterPower(ON);
	Set(PRN_PW, 1);
	//printk("user_prn_write _write_fn call  BHeight=%d , BHeight*Pwidth =%d\n", BHeight, BHeight*Pwidth);
	if(lock==0){
		EPIT_SPI_SEND();
		Set(LAT,0);
	        Set(LAT,1);
		//printk("\nfront->BUF_LEN=%d \n",front->BUF_LEN);
		epit_timer(front->BUF_LEN);
		//	epit_timer(BHeight);
		lock=1;
	}
	return 0;
}

//static int  __devinit thprn_probe(struct spi_device *spi)
static int  thprn_probe(struct spi_device *spi)
{

	pspi = spi;
	printk("printer >>>>>>>>>>> probing");
	printk("\n STARTING Printer %s LINE=%d\n",__func__,__LINE__);
	PrinterInit();
return 0;
}

//static int __devexit thprn_remove(struct spi_device *spi)
static int thprn_remove(struct spi_device *spi)
{
//return spi_unregister_driver(&PrinterDriver);
return 0; 
}

//table id added by arun analogics
static const struct of_device_id printer_match_table[] = {
		{.compatible = "Analogics,2T", },
		{ },
};

static struct spi_driver PrinterDriver = {

        .driver = {
                .name   = "thprn",
                .bus    = &spi_bus_type,
                .owner  = THIS_MODULE,
		//table id added by arun analogics
		.of_match_table = of_match_ptr(printer_match_table),
        },
	.probe = thprn_probe,
	//.remove = __devexit_p(thprn_remove),
	.remove = thprn_remove,
        
};


static const struct file_operations user_prn_file_fops = { /* Operations for controling the device */
         .owner             = THIS_MODULE,
         //.read              = user_prn_read,
         .write             = user_prn_write,
	 //.unlocked_ioctl    = user_prn_ioctl,
};

static struct miscdevice thprn = { /*Creating a misc device called thprn */
         MISC_DYNAMIC_MINOR,
	"ThPrnDev",	          /* Name sets here,  will be shown in /dev */     
         &user_prn_file_fops
};


static int __init thprn_init(void)
{
	int ret=-1;

	ret=spi_register_driver(&PrinterDriver);
        ret=-1;
        ret = misc_register(&thprn); /* Registering the misc device */

 return ret;
}

static void __exit thprn_exit(void)
{
        spi_unregister_driver(&PrinterDriver);
        misc_deregister(&thprn); // Unregistering the device 
}

module_init(thprn_init);
module_exit(thprn_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("V.K.Kishore.K <kvkishore12@gmail.com>");
MODULE_DESCRIPTION("TERMAL 2-INCH PRINTER Module");
MODULE_VERSION("Ver 0.1");

