#ifndef __ATIL_AM335X_GPIO_H
#define __ATIL_AM335X_GPIO_H
                

#include <asm/gpio.h>
//#include <mach/gpio.h>
#include <linux/gpio.h>
#include <asm/io.h>


#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

static int Gpio_set_value(unsigned gpio,const char *label,int Level,int Val){

         if(gpio_request(gpio,label)!=0)
		return(-1);
         if(gpio_direction_output(gpio,Level)!=0)
		return(-2);

         gpio_set_value(gpio,Val);
 
	 printk("\nPin=%s Level=%d Value=%d\n\r",label,Level,Val);	
return 0;
}

static int Gpio_get_value(unsigned gpio,const char *label)
{
         if(gpio_request(gpio,label)!=0)
		return(-1);
         if(gpio_direction_input(gpio)!=0)
		return(-2);
return(gpio_get_value(gpio));
}
#endif
