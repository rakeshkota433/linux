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
#include <mach/hardware.h>
#include <asm/delay.h>
#include <asm/pmu.h>
#include "Am335x_gpio.h"
//#include "../../drivers/staging/iio/iio.h"

/************************************************************************************/

/************************************************************************************/
#define IMAGE_SIZE 4194304
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

//PRINTER STROBES
#define STB1   	GPIO_TO_PIN(1, 17) //GPMC_A1  select as GPIO MOD7 in BANK1 of 17th pin 
#define STB2   	GPIO_TO_PIN(1, 18) //GPMC_A2  select as GPIO MOD7 in BANK1 of 18th pin 
#define STB3   	GPIO_TO_PIN(1, 19) //GPMC_A3  select as GPIO MOD7 in BANK1 of 19th pin 
#define STB4   	GPIO_TO_PIN(1, 20) //GPMC_A4  select as GPIO MOD7 in BANK1 of 20th pin 
#define STB5   	GPIO_TO_PIN(1, 21) //GPMC_A5  select as GPIO MOD7 in BANK1 of 21th pin 
#define STB6   	GPIO_TO_PIN(1, 22) //GPMC_A6  select as GPIO MOD7 in BANK1 of 22th pin 

//STEPPER MOTOR PINS  
#define ENA1   	GPIO_TO_PIN(1, 23) //GPMC_A7  select as GPIO MOD7 in BANK1 of 23th pin 
#define ENA2   	GPIO_TO_PIN(1, 24) //GPMC_A8  select as GPIO MOD7 in BANK1 of 24th pin 
#define IN1    	GPIO_TO_PIN(1, 25) //GPMC_A9  select as GPIO MOD7 in BANK1 of 25th pin 
#define IN2    	GPIO_TO_PIN(1, 26) //GPMC_A10 select as GPIO MOD7 in BANK1 of 26th pin 

//PRINTER CONTROL PINS
#define PRN_PW  GPIO_TO_PIN(1, 27) //GPMC_A11 select as GPIO MOD7 in BANK1 of 27th pin
#define LAT  	GPIO_TO_PIN(2,  2) //GPMC_ALE select as GPIO MOD7 in BANK2 of 2nd  pin

//#define MOSI    GPIO_TO_PIN(0,  3) //SPI0_D0  select as GPIO MOD7 in BANK0 of 3rd  pin

#define Set(X,V) gpio_set_value(X,V)

#define CS_CHANGE(Pxfer)        ((Pxfer).cs_change = 0)

#define Pwidth  48

/****************************************************************************************/
/*											*/
/****************************************************************************************/
//static __devinit thprn_probe(struct spi_device );

//This for ADC RAW RED FUNCTION WICH IS EXPORTED THERE TO USE HERE
//static int tiadc_read_raw(struct iio_dev,struct iio_chan_spec const *,int *,int *,long);
int tiadc_read_raw(struct iio_dev,struct iio_chan_spec const *,int *,int *,long);

/****************************************************************************************/
		
const unsigned char bar[]={0x06,0xCC,0x06,0x6C,0x06,0x66,0x04,0x98,0x04,0x8C,
	0x04,0x4C,0x04,0xC8,0x04,0xC4,0x04,0x64,0x06,0x48,0x06,0x44,0x06,0x24,0x05,
	0x9C,0x04,0xDC,0x04,0xCE,0x05,0xCC,0x04,0xEC,0x04,0xE6,0x06,0x72,0x06,0x5C,
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

char print_buf[IMAGE_SIZE];
//char print_buf[4000];
int lCnt=0;
int Buf1Cnt;
unsigned char mot_flag;

struct prn_bus{
        struct spi_message    Pmsg;
        struct spi_transfer   Pxfer;
};

struct spi_device pl,*pspi;

//static void Format_spi_data(struct device *dev,unsigned char *print_buf)
static void Format_spi_data(struct device *dev)
{
	int status;
        struct spi_device *spi =  to_spi_device(dev);
        struct prn_bus    *bus =  kzalloc(sizeof *bus,GFP_KERNEL);



        spi_message_init(&bus->Pmsg);
        bus->Pxfer.tx_buf = &print_buf[lCnt];
        bus->Pxfer.len = Pwidth;
	CS_CHANGE(bus->Pxfer);
        spi_message_add_tail(&bus->Pxfer,&bus->Pmsg);
        //disable_irq(&bus->spi->irq);
        status = spi_sync(spi,&bus->Pmsg);
	//enable_irq(&bus->spi->irq);
        kfree(bus);

	lCnt+=Pwidth;

      Set(LAT,0);
      Set(LAT,1);


}

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
      Set(LAT,1);


}
/*
void DotsInEachLinelogo(void)
{
      unsigned char BytCnt=0;
      
      //for(BytCnt=0;BytCnt<48;BytCnt++,lCnt++)
      //print_buf1[BytCnt] = print_buf[lCnt];

      //memcpy(&print_buf1[BytCnt],&print_buf[lCnt],48);

      Buf1Cnt = 48;
      //Format_spi_data(&pspi->dev);

      Set(LAT,0);
      Set(LAT,1);
}



void DotsInEachLinelogo_char(unsigned char *dot_buf)
{
	unsigned char BytCnt;

	for(BytCnt=0;BytCnt<48;BytCnt++)
		print_buf[BytCnt] = dot_buf[BytCnt];
         
	//Format_spi_data_char(&pspi->dev);
}
*/

void PrinterPower(unsigned char State)
{
        if(State == ON)
                Set(PRN_PW, 1);
        if(State == OFF)
                Set(PRN_PW, 0);
}

void PrinterInit(void)
{
   int Ret=9;
  
        /*Strobe Pins set as gpio mode*/
        Ret=Gpio_set_value(STB1,"STROB1",1,0);
	//printk("STB1 Ret=%d\n",Ret);
	Ret=Gpio_set_value(STB2,"STROB2",1,0);
	//printk("STB2 Ret=%d\n",Ret);
        Ret=Gpio_set_value(STB3,"STROB3",1,0);
	//printk("STB3 Ret=%d\n",Ret);
        Ret=Gpio_set_value(STB4,"STROB4",1,0);
	//printk("STB4 Ret=%d\n",Ret);
        Ret=Gpio_set_value(STB5,"STROB5",1,0);
	//printk("STB5 Ret=%d\n",Ret);
        Ret=Gpio_set_value(STB6,"STROB6",1,0);
	//printk("STB6 Ret=%d\n",Ret);
        /*Stepper Motor Pins set as gpio mode*/
        Ret=Gpio_set_value(IN1 ,"MTA",1,0);
	//printk("IN1 Ret=%d\n",Ret);
        Ret=Gpio_set_value(IN2 ,"MTA1",1,0);
	//printk("IN2 Ret=%d\n",Ret);
        Ret=Gpio_set_value(ENA1,"MTB",1,0);
	//printk("ENA1 Ret=%d\n",Ret);
        Ret=Gpio_set_value(ENA2,"MTB1",1,0);
	//printk("ENA2 Ret=%d\n",Ret);
        /*Printer Control pins set as gpio mode*/
        Ret=Gpio_set_value(PRN_PW,"VH",1,0);
	//printk("PRN_PW Ret=%d\n",Ret);
        Ret=Gpio_set_value(LAT,"LATCH",1,0);
	//printk("LAT Ret=%d\n",Ret);
	//while(1)
	//{
	//	Set(STB1,1);
	//	__udelay(1000);
	//	Set(STB1,0);
	//	__udelay(1000);
	//}
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

void Rotate(int Step,int Sdly)
{
      if(Sdly<0)
	Sdly=1050; 
	//Sdly=900;	//Ravindra
      switch(Step)
      {
	//This was commneted by kishore becoze this steps are inverted buffer routiens 
	//case 1: Set(IN2,1); Set(ENA1,0); Set(IN1,0); Set(ENA2,1); __udelay(Sdly);break;
	//case 2: Set(IN2,1); Set(ENA1,1); Set(IN1,0); Set(ENA2,0); __udelay(Sdly);break;
	//case 3: Set(IN2,0); Set(ENA1,1); Set(IN1,1); Set(ENA2,0); __udelay(Sdly);break;
	//case 4: Set(IN2,0); Set(ENA1,0); Set(IN1,1); Set(ENA2,1); __udelay(Sdly);break; 
	
	case 1: Set(ENA1,0); Set(ENA2,1); Set(IN1,1); Set(IN2,0); __udelay(Sdly);break;
        case 2: Set(ENA1,0); Set(ENA2,1); Set(IN1,0); Set(IN2,1); __udelay(Sdly);break;
        case 3: Set(ENA1,1); Set(ENA2,0); Set(IN1,0); Set(IN2,1); __udelay(Sdly);break;
	case 4: Set(ENA1,1); Set(ENA2,0); Set(IN1,1); Set(IN2,0); __udelay(Sdly);break;//Sdly
      }
}

void Cycle(int Type,int Dly)
{
   if(Type == F_HALF)
   {  
	Rotate(1,Dly);
	Rotate(2,Dly);
   }
   if(Type == S_HALF)
   {
        Rotate(3,Dly);
        Rotate(4,Dly);
   }

   if(Type == RF_HALF)
   {
        Rotate(3,Dly);
        Rotate(2,Dly);
   }
   if(Type == RS_HALF)
   {
        Rotate(1,Dly);
        Rotate(4,Dly);
   }
   if(Type == R_FULL_)
   {
        Rotate(3,Dly);
        Rotate(2,Dly);
        Rotate(1,Dly);
        Rotate(4,Dly);
   }		

   if(Type == _FULL_)
   {
	Rotate(1,Dly);
	Rotate(2,Dly);
	Rotate(3,Dly);
	Rotate(4,Dly);
   }


}

void Enablestrobes(int X)
{
    switch(X)
    {
      case 1: Set(STB1,1);Set(STB3,1);Set(STB5,1);	break;

      case 2: Set(STB2,1);Set(STB4,1);Set(STB6,1);	break;

    }

}

void Disablestrobes(int X)
{
    switch(X)
    {
      case 1: Set(STB1,0);Set(STB3,0);Set(STB5,0);	break;

      case 2: Set(STB2,0);Set(STB4,0);Set(STB6,0);      break;

      case 5: Set(STB1,0);Set(STB3,0);Set(STB5,0);
	      Set(STB2,0);Set(STB4,0);Set(STB6,0);      break;

    }

}

void ForwardPaper(void)
{
	int Sdly;
	Sdly=900;

	if(!mot_flag)
	{
		Enablestrobes(1);
		Rotate(STEP1,Sdly); 
		Disablestrobes(5);
		__udelay(100);
		Enablestrobes(2);
                Rotate(STEP2,Sdly);
		Disablestrobes(5);
		__udelay(100);

	}
	else
	{
		Enablestrobes(1);
                Rotate(STEP3,Sdly);
		Disablestrobes(5);
		__udelay(100);
		Enablestrobes(2);
                Rotate(STEP4,Sdly); 
		Disablestrobes(5);
		__udelay(100);

	}
}


void RewindPaper()
{
   Cycle(RF_HALF,1250);
   Cycle(RS_HALF,1250);

}

static int user_prn_read(struct file *file, char *buf, unsigned char count, loff_t *ppos)
{
	//struct iio_dev adcdev;
	//struct iio_chan_spec const chan;
        int val,val2;
	long mask;

        //wait_event_interruptible(prin_queue,wevent!=0);
        copy_to_user(buf,"001",3);

	//tiadc_read_raw(adcdev,&chan,&val,&val2,mask);
        //while(1)
        {
                PrinterPower(ON);
                Cycle(_FULL_,1250);
                PrinterPower(OFF);
        }
	printk("\n READ SUCCESS  Valu=%d\n",val);

        return 0;
}

static int user_prn_ioctl(struct file *file, unsigned char count, char *buf)
{

    		int io_i,io_j,io_h,temp_io;
	    	//char ioctl_buff[4000];
    		char ioctl_buff1[4000];
                char ioctl_buff2[50];

        	//memset(print_buf,0x00,sizeof(print_buf));	//Ravindra
        	//memset(ioctl_buff,0x00,sizeof(ioctl_buff));
        	memset(ioctl_buff1,0x00,sizeof(ioctl_buff1));
        	memset(ioctl_buff2,0x00,sizeof(ioctl_buff2));

        	copy_from_user(ioctl_buff1,buf,count);

                PrinterPower(ON);
                mot_flag=0x00;
                lCnt=0; 
		Cycle(_FULL_,3000); 
                for(io_i = 0;io_i < count; )
                {
                    temp_io = io_i;
			
                      for(io_h = 0,io_j = io_i;io_j < (temp_io+48);io_j++,io_h++)
                      {
                      	//ioctl_buff[io_j - temp_io]=ioctl_buff1[io_j];  //Ravindra
                      	//ioctl_buff2[io_h]=ioctl_buff[io_j - temp_io];
                      	ioctl_buff2[io_h]=ioctl_buff1[io_j];
                      }
		      mot_flag =mot_flag & 0x01;
		      Format_spi_data_char(&pspi->dev,ioctl_buff2);
                      //DotsInEachLinelogo_char(ioctl_buff2);
                      ForwardPaper();
		      mot_flag =~ mot_flag;
                      io_i = io_j;
                }
/*********************************************/
//	Cycle(_FULL_,1250);             //1250 work for smooth  dalay
  //      Cycle(_FULL_,1250);             //1250 work for smooth  dalay
    //    Cycle(_FULL_,1250);             //1250 work for smooth  dalay
	 Cycle(_FULL_,3000);
        Disablestrobes(3);              //DISABLE ALL STROBES
        Set(IN2,0);                     //DISABLE MOTOR PINS
        Set(ENA1,0);                    //DISABLE MOTOR PINS
        Set(IN1,0);                     //DISABLE MOTOR PINS
        Set(ENA2,0);                    //DISABLE MOTOR PINS
        PrinterPower(OFF);              //PRINTER POWER OFF

  	return 0;
}

//static int user_prn_write(struct file * file,unsigned char *print_buf,unsigned int BHeight, loff_t *loft) {
static int user_prn_write(struct file * file,unsigned char *buf,unsigned int BHeight, loff_t *loft) {

        int Dlcnt=0; 
        
	copy_from_user(print_buf,buf, BHeight*Pwidth );

	lCnt=0;
        Dlcnt=0; 
	PrinterPower(ON);
	mot_flag=0x00;
	Cycle(_FULL_,3000); 
        while(Dlcnt < BHeight)
        {
		Format_spi_data(&pspi->dev);
		mot_flag =mot_flag & 0x01;
		ForwardPaper();
		Dlcnt++;
		mot_flag =~ mot_flag;
        }

//	Cycle(_FULL_,1250);        	//1250 work for smooth  dalay
//	Cycle(_FULL_,1250);        	//1250 work for smooth  dalay
//	Cycle(_FULL_,1250);        	//1250 work for smooth  dalay
	 Cycle(_FULL_,3000);
        Disablestrobes(3); 		//DISABLE ALL STROBES
	Set(IN2,0); 			//DISABLE MOTOR PINS
	Set(ENA1,0); 			//DISABLE MOTOR PINS
	Set(IN1,0); 			//DISABLE MOTOR PINS
	Set(ENA2,0);  			//DISABLE MOTOR PINS
	PrinterPower(OFF);		//PRINTER POWER OFF

        return 0;
}

static int  __devinit thprn_probe(struct spi_device *spi)
{

	pspi = spi;
	printk("\n Printer %s LINE=%d\n",__func__,__LINE__);
	PrinterInit();
return 0;
}

static int __devexit thprn_remove(struct spi_device *spi)
{
//return spi_unregister_driver(&PrinterDriver);
return 0; 
}

static struct spi_driver PrinterDriver = {

        .driver = {
                .name   = "thprn",
                .bus    = &spi_bus_type,
                .owner  = THIS_MODULE,
        },
	.probe = thprn_probe,
	.remove = __devexit_p(thprn_remove),
        
};


static const struct file_operations user_prn_file_fops = { /* Operations for controling the device */
         .owner             = THIS_MODULE,
         .read              = user_prn_read,
         .write             = user_prn_write,
	 .unlocked_ioctl    = user_prn_ioctl,
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

