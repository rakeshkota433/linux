/*
 *  linux/arch/arm/plat-mxc/epit.c
 *
 *  Copyright (C) 2010 Sascha Hauer <s.hauer@pengutronix.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#define EPITCR		0x00
#define EPITSR		0x04
#define EPITLR		0x08
#define EPITCMPR	0x0c
#define EPITCNR		0x10

#define EPITCR_EN			(1 << 0)
#define EPITCR_ENMOD			(1 << 1)
#define EPITCR_OCIEN			(1 << 2)
#define EPITCR_RLD			(1 << 3)
#define EPITCR_PRESC(x)			(((x) & 0xfff) << 4)
#define EPITCR_SWR			(1 << 16)
#define EPITCR_IOVW			(1 << 17)
#define EPITCR_DBGEN			(1 << 18)
#define EPITCR_WAITEN			(1 << 19)
#define EPITCR_RES			(1 << 20)
#define EPITCR_STOPEN			(1 << 21)
#define EPITCR_OM_DISCON		(0 << 22)
#define EPITCR_OM_TOGGLE		(1 << 22)
#define EPITCR_OM_CLEAR			(2 << 22)
#define EPITCR_OM_SET			(3 << 22)
#define EPITCR_CLKSRC_OFF		(0 << 24)
#define EPITCR_CLKSRC_PERIPHERAL	(1 << 24)
#define EPITCR_CLKSRC_REF_HIGH		(1 << 25) //analogic
#define EPITCR_CLKSRC_REF_LOW		(3 << 24)

#define EPITSR_OCIF			(1 << 0)

#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/clockchips.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <asm/mach/time.h>

#include "common.h"
#include "hardware.h"


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>
#include <linux/platform_data/i2c-imx.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/reboot.h>
#include <linux/printk.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/clk.h>
#include <linux/time.h>
#include "Am335x_gpio.h"
#include <linux/sched_clock.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include <linux/workqueue.h>

//############################################################

#define gpio_epit     GPIO_TO_PIN(0,4) //MX6UL_PAD_GPIO1_IO04__GPIO1_IO04
#define Set(X,V) gpio_set_value(X,V) //MX6UL_PAD_UART3_RX_DATA__GPIO1_IO25

static int E1_V,E2_V,IN1_V,IN2_V;
static int STB1_V,STB2_V,STB3_V,STB4_V,STB5_V,STB6_V ;
static int BHeight;
extern void PrinterPower(unsigned char State);
extern void Latch(void);
extern void EPIT_SPI_SEND(void);
#define ON         1
#define OFF        0
//###################################analogic new
//#include <linux/timer.h>
//static struct timer_list printer_timer;
extern int epit_timer(int );
//struct delayed_work work;
struct delayed_work work_poweroff;
struct delayed_work work_spi_send;

unsigned long delay;
static void epit_spi_work(void);
static void poweroff_work(void);
static void spi_send(void);
static void spi_send(void)
{
      EPIT_SPI_SEND();
}


//void my_timer_callback( unsigned long data )
//{
//	EPIT_SPI_SEND();
//	udelay(100);
//	epit_timer(BHeight);

//}
//#####################################



#define STB1   	GPIO_TO_PIN(3, 21) //MX6UL_PAD_CSI_DATA00__GPIO4_IO21
#define STB2   	GPIO_TO_PIN(3, 22) //MX6UL_PAD_CSI_DATA01__GPIO4_IO22
#define STB3   	GPIO_TO_PIN(3, 23) //MX6UL_PAD_CSI_DATA02__GPIO4_IO23
#define STB4   	GPIO_TO_PIN(3, 24) //MX6UL_PAD_CSI_DATA03__GPIO4_IO24
#define STB5   	GPIO_TO_PIN(3, 26) //MX6UL_PAD_CSI_DATA05__GPIO4_IO26
#define STB6   	GPIO_TO_PIN(3, 13) //MX6UL_PAD_NAND_CE0_B__GPIO4_IO13 






//STEPPER MOTOR FOR DRV8833 BY RAKESH ANALOGICS
#define ENA1     GPIO_TO_PIN(0, 26) //MX6UL_PAD_UART3_CTS_B__GPIO1_IO26
#define ENA2     GPIO_TO_PIN(1, 8) //MX6UL_PAD_ENET2_RX_DATA0__GPIO2_IO08
#define IN1      GPIO_TO_PIN(1, 9) //MX6UL_PAD_ENET2_RX_DATA1__GPIO2_IO09
#define IN2      GPIO_TO_PIN(0, 27) //MX6UL_PAD_UART3_RTS_B__GPIO1_IO27

//#define PRN_PW  GPIO_TO_PIN(3,  20) //MX6UL_PAD_CSI_HSYNC__GPIO4_IO20


//##############################################################

static struct clock_event_device clockevent_epit;
static enum clock_event_mode clockevent_mode = CLOCK_EVT_MODE_UNUSED;//analogics
//static enum clock_event_mode clockevent_mode = CLOCK_EVT_MODE_ONESHOT;
static void __iomem *timer_base;
void epit_off(void);

static inline void epit_irq_disable(void)
{
	u32 val;

	val = __raw_readl(timer_base + EPITCR);
	val &= ~EPITCR_OCIEN;
	__raw_writel(val, timer_base + EPITCR);
}

static inline void epit_irq_enable(void)
{
	u32 val;

	val = __raw_readl(timer_base + EPITCR);
	val |= EPITCR_OCIEN;
	__raw_writel(val, timer_base + EPITCR);
}

static void epit_irq_acknowledge(void)
{
	//static int k;	
	__raw_writel(EPITSR_OCIF, timer_base + EPITSR);
	//printk("epit_irq_acknowledge = %d" ,k+1);
}
static int __init epit_clocksource_init(struct clk *timer_clk)
{
	unsigned int c = clk_get_rate(timer_clk);
	 void __iomem *reg = timer_base + EPITCNR ;
	printk("clk_get_rate =%d ",c);

	return clocksource_mmio_init(reg, "epit", c, 200, 32,
			clocksource_mmio_readl_down);

/*	return clocksource_mmio_init(timer_base + EPITCNR, "epit", c, 200, 32,
			clocksource_mmio_readl_down);*/
}

/* clock event */

static int epit_set_next_event(unsigned long evt,
			      struct clock_event_device *unused)
{
	unsigned long tcmp;

	tcmp = __raw_readl(timer_base + EPITCNR);
	printk("epit_set_next_event is called");

	__raw_writel(tcmp - evt, timer_base + EPITCMPR);

	return 0;
}

static void epit_set_mode(enum clock_event_mode mode,
				struct clock_event_device *evt)
{
	unsigned long flags;
	//mode = CLOCK_EVT_MODE_ONESHOT;

	/*
	 * The timer interrupt generation is disabled at least
	 * for enough time to call epit_set_next_event()
	 */
	local_irq_save(flags);

	/* Disable interrupt in GPT module */
	epit_irq_disable();
	//printk("mode  =%x ,clockevent_mode =%x", mode,clockevent_mode);
	if (mode != clockevent_mode) {
		/* Set event time into far-far future */

		/* Clear pending interrupt */
		epit_irq_acknowledge();
	}

	/* Remember timer mode */
	clockevent_mode = mode;
	local_irq_restore(flags);
	printk("EPIT _mode  =%x ,clockevent_mode =%x\n", mode,clockevent_mode);
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		printk(KERN_ERR "epit_set_mode: Periodic mode is not "
				"supported for i.MX EPIT\n");
		break;
	case CLOCK_EVT_MODE_ONESHOT:
	/*
	 * Do not put overhead of interrupt enable/disable into
	 * epit_set_next_event(), the core has about 4 minutes
	 * to call epit_set_next_event() or shutdown clock after
	 * mode switching
	 */
		local_irq_save(flags);
		printk("\n epit_irq_enable \n");
		epit_irq_enable();
		local_irq_restore(flags);
		break;
	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_RESUME:
		/* Left event sources disabled, no more interrupts appear */
		break;
	}
}

/*
 * IRQ handler for the timer
 */

static void epit_spi_work()
{
 //EPIT_SPI_SEND();  
 Latch();
// epit_timer(BHeight);
}
static void poweroff_work(void)
{
                         PrinterPower(OFF);              //PRINTER POWER OFF
}
 void epit_off(void)
{

   __raw_writel(0x00000000, timer_base + EPITCR);
}
EXPORT_SYMBOL(epit_off);
int strob(int STROB)
{
	switch(STROB)
	{
		case 0: STB1_V=1;STB3_V=1;STB5_V=1;STB2_V=0;STB4_V=0;STB6_V=0;STROB=1;  break;

		case 1: STB2_V=1;STB4_V=1;STB6_V=1;STB1_V=0;STB3_V=0;STB5_V=0;STROB=0; break;
	}
                Set(STB1,STB1_V);Set(STB3,STB3_V);Set(STB5,STB5_V);Set(STB2,STB2_V);Set(STB4,STB4_V);Set(STB6,STB6_V);
}
int step(void)
{
	static int STEP; 
	switch(STEP)
	{
		case 0:E1_V=1;E2_V=0;IN1_V=0;IN2_V=1;STEP=1;break;
		case 1:E1_V=1;E2_V=1;IN1_V=0;IN2_V=1;STEP=2;break;

		case 2:E1_V=0;E2_V=1;IN1_V=0;IN2_V=1;STEP=3;break;
		case 3:E1_V=0;E2_V=1;IN1_V=1;IN2_V=1;STEP=4;break;

		case 4:E1_V=0;E2_V=1;IN1_V=1;IN2_V=0;STEP=5;break;
		case 5:E1_V=1;E2_V=1;IN1_V=1;IN2_V=0;STEP=6;break;

		case 6:E1_V=1;E2_V=0;IN1_V=1;IN2_V=0;STEP=7;break;
		case 7:E1_V=1;E2_V=0;IN1_V=1;IN2_V=1;STEP=0;break;
	}

	Set(ENA1,E1_V); Set(ENA2,E2_V); Set(IN1,IN1_V); Set(IN2,IN2_V);
}	


static irqreturn_t epit_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = &clockevent_epit;
	static int STEP,count,STROB,Num_line;
	epit_irq_acknowledge();
	//static int STEP;
	//	printk("i am in epit_timer_interrupt handlar");
	evt->event_handler(evt);
	
	if(count==3){
				Set(STB1,0);Set(STB3,0);Set(STB5,0);Set(STB2,0);Set(STB4,0);Set(STB6,0);
		count=0;
		if(Num_line<BHeight){
			//		schedule_delayed_work(&work,delay);
			step();
                        strob(0);
		//	schedule_work(&work);
			Latch();
			Num_line++;
			//			return IRQ_HANDLED;
		}else{
			Num_line=1;
			Set(ENA1,0); Set(ENA2,0); Set(IN1,0); Set(IN2,0);
			 schedule_work(&work_poweroff);
		}


	}else if(count==2){
	count=count+1;
	step();
      // if(Num_line<BHeight)
        //schedule_work(&work_spi_send);	
	}
	else if(count==1){
		//Set(STB1,0);Set(STB3,0);Set(STB5,0);Set(STB2,0);Set(STB4,0);Set(STB6,0);
		strob(1);
		step();
		count=count+1;
	if(Num_line<BHeight)
        schedule_work(&work_spi_send);	

	}else{
		step ();
		count=count+1;
	}

	return IRQ_HANDLED;
}

static struct irqaction epit_timer_irq = {
	.name		= "i.MX EPIT Timer Tick",
	.flags		= IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= epit_timer_interrupt,
};

static struct clock_event_device clockevent_epit = {
	.name		= "epit",
	.features	= CLOCK_EVT_FEAT_ONESHOT,
	.set_mode	= epit_set_mode,
	.set_next_event	= epit_set_next_event,
	.rating		= 200,
};

static int __init epit_clockevent_init(struct clk *timer_clk)
{
	clockevent_epit.cpumask = cpumask_of(0);
	clockevents_config_and_register(&clockevent_epit,
					clk_get_rate(timer_clk),
					0x800, 0xfffffffe);

	return 0;
}

static int  epit_probe(struct platform_device *pdev)
{
	 // struct regmap *epit;
          int irq;
	  struct device_node *ep;
	 struct clk *timer_clk;	
         delay = usecs_to_jiffies(10);
        /* Get SNVS register Page */
        ep = pdev->dev.of_node;
        if (!ep)
                return -ENODEV;
	timer_base = of_iomap(ep, 0);
        WARN_ON(!timer_base);
        irq = irq_of_parse_and_map(ep, 0);
	printk("epit->irq =%d",irq);

        timer_clk = of_clk_get_by_name(ep, "perclk");

	clk_prepare_enable(timer_clk);
	Gpio_set_value(gpio_epit,"gpio_epit",1,0);
	 
	Gpio_set_value(IN1 ,"MTA",1,0);
       	Gpio_set_value(IN2 ,"MTA1",1,0);
        Gpio_set_value(ENA1,"MTB",1,0);
        Gpio_set_value(ENA2,"MTB1",1,0);
	//Gpio_set_value(PRN_PW,"VH",1,1);
	
        Gpio_set_value(STB1,"STROB1",1,0);
	Gpio_set_value(STB2,"STROB2",1,0);
        Gpio_set_value(STB3,"STROB3",1,0);
        Gpio_set_value(STB4,"STROB4",1,0);
        Gpio_set_value(STB5,"STROB5",1,0);
        Gpio_set_value(STB6,"STROB6",1,0);
        //setup_timer(&printer_timer, my_timer_callback, 0);
        //INIT_DELAYED_WORK(&work, epit_spi_work);
        INIT_DELAYED_WORK(&work_poweroff, poweroff_work);
        INIT_DELAYED_WORK(&work_spi_send, spi_send);

	//Set(gpio_epit,1);
	/*
	 * Initialise to a known state (all timers off, and timing reset)
	 */
	__raw_writel(0x0000000a, timer_base + EPITCR);
	printk("\n EPITLR = %x\n" ,  __raw_readl(timer_base + EPITLR));
	__raw_writel(0x00003A98, timer_base + EPITLR); //0x00003A98
	printk("\n EPITCNR = %x\n" ,  __raw_readl(timer_base + EPITCNR));
	__raw_writel(0x00000000, timer_base + EPITCMPR);
/*	__raw_writel(EPITCR_EN | EPITCR_CLKSRC_REF_HIGH | EPITCR_WAITEN |EPITCR_IOVW | EPITCR_OM_TOGGLE |EPITCR_RLD | EPITCR_OCIEN | EPITCR_ENMOD,
			timer_base + EPITCR);*/
	__raw_writel(EPITCR_EN | EPITCR_OCIEN | EPITCR_CLKSRC_PERIPHERAL | EPITCR_WAITEN | EPITCR_OM_CLEAR | EPITCR_RLD  | EPITCR_ENMOD | EPITCR_IOVW ,timer_base + EPITCR);

	printk("\n EPITCR = %x\n" ,  __raw_readl(timer_base + EPITCR));
	/* init and register the timer to the framework */
	epit_clocksource_init(timer_clk);
	epit_clockevent_init(timer_clk);

	/* Make irqs happen */
	setup_irq(irq, &epit_timer_irq);


//printk("EPIT >>>>>>>>>>> probing");
//	epit_timer_init(epit,irq);
return 0;
}
int epit_timer(int value)
{

	BHeight=value;
	
//	        printk("i am in epit_timer handlar");
//	__raw_writel(0x0000000a, timer_base + EPITCR);
//	__raw_writel(0x00003A98, timer_base + EPITLR); //0x00003A98
//	__raw_writel(0x00000000, timer_base + EPITCMPR);
		strob(0);
		step();
	__raw_writel(EPITCR_EN | EPITCR_OCIEN | EPITCR_CLKSRC_PERIPHERAL | EPITCR_WAITEN | EPITCR_OM_CLEAR | EPITCR_RLD  | EPITCR_ENMOD | EPITCR_IOVW ,timer_base + EPITCR);
}
EXPORT_SYMBOL_GPL(epit_timer);
static int epit_remove(struct platform_device *pdev)
{
return 0; 
}

static const struct of_device_id epit_match_table[] = {
		{.compatible = "Analogics,EPIT", },
		{ },
};
MODULE_DEVICE_TABLE(of, epit_match_table);

static struct platform_driver EpitDriver = {

        .driver = {
                .name   = "Analogic-Epit",
                .owner  = THIS_MODULE,
		.of_match_table = of_match_ptr(epit_match_table),
        },
	.probe = epit_probe,
	//.remove = __devexit_p(thprn_remove),
	.remove = epit_remove,
        
};


static int __init epit_imx_init(void)
{
	return platform_driver_register(&EpitDriver);
}
subsys_initcall(epit_imx_init);

static void __exit epit_imx_exit(void)
{
	platform_driver_unregister(&EpitDriver);
}
module_exit(epit_imx_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAKESH");
MODULE_DESCRIPTION("Epit Module");
MODULE_VERSION("Ver 0.1");

