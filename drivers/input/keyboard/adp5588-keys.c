/*
 * File: drivers/input/keyboard/adp5588_keys.c
 * Description:  keypad driver for ADP5588 and ADP5587
 *		 I2C QWERTY Keypad and IO Expander
 * Bugs: Enter bugs at http://blackfin.uclinux.org/
 *
 * Copyright (C) 2008-2010 Analog Devices Inc.
 * Licensed under the GPL-2 or later.
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/errno.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/slab.h>

#include <linux/i2c/adp5588.h>

//added by arun analogics
//**************************************************/
#include <linux/of.h>
#include <linux/input/matrix_keypad.h>
#include <linux/delay.h>
#include "Am335x_gpio.h"
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/pwm.h>



//#define CONFIG_PM 1
//#include <asm-generic/delay.h>
#define Set(X,V) gpio_set_value(X,V)
#define BUZZER_CNT 60    
#define BUZZER_DELAY 200 
//#define BUZZER GPIO_TO_PIN(3, 19) //MX6UL_PAD_CSI_VSYNC__GPIO4_IO19 
/**************************************************/
void BUZZER_EXPORT(void)
{
//Gpio_set_value(BUZZER,"BUZZER",1,0);
}

/* Key Event Register xy */
#define KEY_EV_PRESSED		(1 << 7)
#define KEY_EV_MASK		(0x7F)

#define KP_SEL(x)		(0xFFFF >> (16 - x))	/* 2^x-1 */

#define KEYP_MAX_EVENT		10

#define ON 1
#define OFF 0
#define MIN_TIMER 10
#define MAX_TIMER 1000

/*
 * Early pre 4.0 Silicon required to delay readout by at least 25ms,
 * since the Event Counter Register updated 25ms after the interrupt
 * asserted.
 */

static int timerset = MAX_TIMER;
static int flag = 0;
unsigned short int  timer_expired = 0;
unsigned short int  prev_finvalue = 0;

static int my_timer_callback( unsigned long data );
static struct timer_list my_timer;

static struct timer_list my_timer_bazzer;
static int my_timer_callback_bazzer( unsigned long data );


#define WA_DELAYED_READOUT_REVID(rev)		((rev) < 4)

struct adp5588_kpad {
	struct i2c_client *client;
	struct input_dev *input;
	struct delayed_work work;
	unsigned long delay;
	unsigned short keycode[ADP5588_KEYMAPSIZE];
	const struct adp5588_gpi_map *gpimap;
	unsigned short gpimapsize;
#ifdef CONFIG_GPIOLIB
	unsigned char gpiomap[ADP5588_MAXGPIO];
	bool export_gpio;
	struct gpio_chip gc;
	struct mutex gpio_lock;	/* Protect cached dir, dat_out */
	u8 dat_out[3];
	u8 dir[3];
#endif
};

/********************** ANALOGICS ADP5588 PLATFORM DATA *****************************/

static int secflg,seccnt;
static int buzzerflag = 1;
#define KEY_DUMMY     0xFF

 
/*
static const unsigned short adp5588_keymap[ADP5588_KEYMAPSIZE] = {
        [0]      = KEY_5,               //for %       
        [1]      = KEY_1,               //for (                      
        [2]      = KEY_MINUS,           //for -
        [3]      = KEY_SLASH,           //for /
        [4]      = KEY_RIGHTBRACE,      //for }
        [5]      = KEY_LEFTSHIFT,       // for <<
        [6]      = KEY_DUMMY,
        [7]      = KEY_DUMMY,
        [8]      = KEY_DUMMY,
        [9]      = KEY_DUMMY,
        [10]     = KEY_1,               //for 1   
        [11]     = KEY_2,               //for 2             
        [12]     = KEY_3,               //for 3          
        [13]     = KEY_BACKSLASH,       //for '\'
        [14]     = KEY_COMMA,           //for ,
        [15]     = KEY_BACKSPACE,       //for <-
        [16]     = KEY_DUMMY,
        [17]     = KEY_DUMMY,
        [18]     = KEY_DUMMY,
        [19]     = KEY_DUMMY,
        [20]     = KEY_4,               //for 4 
        [21]     = KEY_5,               //for 5
        [22]     = KEY_6,               //for 6
        [23]     = KEY_4,               //for $
        [24]     = KEY_8,               //for *
        [25]     = KEY_DUMMY,
        [26]     = KEY_DUMMY,
        [27]     = KEY_DUMMY,
        [28]     = KEY_DUMMY,
        [29]     = KEY_DUMMY,
        [30]     = KEY_7,               //for 7
        [31]     = KEY_8,               //for 8
        [32]     = KEY_9,               //for 9
        [33]     = KEY_7,               //for &         
        [34]     = KEY_0,               //for )
        [35]     = KEY_DUMMY,
        [36]     = KEY_DUMMY,
        [37]     = KEY_DUMMY,
        [38]     = KEY_DUMMY,
        [39]     = KEY_DUMMY,
        [40]     = KEY_APOSTROPHE,      //for '     
        [41]     = KEY_0,               //for 0
        [42]     = KEY_RIGHTBRACE,      //for ]
        [43]     = KEY_LEFTBRACE,       //for {
        [44]     = KEY_LEFTBRACE,       //for [
        [45]     = KEY_DUMMY,
        [46]     = KEY_DUMMY,
        [47]     = KEY_DUMMY,
        [48]     = KEY_DUMMY,
        [48]     = KEY_DUMMY,
        [49]     = KEY_DUMMY,
        [50]      = KEY_DUMMY,
        [51]      = KEY_EQUAL,          //for = 
        [52]      = KEY_9,              //for ( 
        [53]      = KEY_3,              //for #
        [54]      = KEY_6, //for ^                   //KEY_ENTER,              
        [55]      = KEY_DUMMY, //free left
        [56]      = KEY_DUMMY, //free left
        [57]      = KEY_DUMMY, //free left
        [58]      = KEY_DUMMY, //free left
        [59]      = KEY_DUMMY, //free left
        [60]      = KEY_DOT,               //55,
        [61]      = KEY_1,
        [62]      = KEY_DOT,

        [63]      = KEY_A,               //46,
        [64]      = KEY_B,
        [65]      = KEY_C,
        [66]      = KEY_D,               //46,
        [67]      = KEY_E,
        [68]      = KEY_F,
        [69]      = KEY_G,               //46,
        [70]      = KEY_H,
        [71]      = KEY_I,
        [72]      = KEY_J,               //46,
        [73]      = KEY_K,
        [74]      = KEY_L,
        [75]      = KEY_M,               //46,
        [76]      = KEY_N,
        [77]      = KEY_O,
        [78]      = KEY_P,               //46,
        [79]      = KEY_Q,
        [80]      = KEY_R,
        [81]      = KEY_T,               //46,
        [82]      = KEY_U,
        [83]      = KEY_V,
        [84]      = KEY_W,               //46,
        [85]      = KEY_X,
        [86]      = KEY_Y,
        [87]      = KEY_Z,
        [88]      = KEY_S,
        [89]      = 57,    //KEY_SPACE,   
};
*/


struct pwm_beeper {
	struct input_dev *input;
	struct pwm_device *pwm;
	unsigned long period;
};

struct pwm_beeper *g_beeper;
int pwm_beeper_key(int value)
{
 	int ret = 0;
//	struct pwm_beeper *beeper = input_get_drvdata(g_beeper->input);
	unsigned long period;
	if (value == 0) {
		pwm_disable(g_beeper->pwm);
	} else {
		//period = HZ_TO_MICROSECONDS(value);
		ret = pwm_config(g_beeper->pwm, 200000 , 400000);
		if (ret)
			return ret;
		ret = pwm_enable(g_beeper->pwm);
		if (ret)
			return ret;
		g_beeper->period = period;
	}

	return 0; 

}


static const unsigned short adp5588_keymap[ADP5588_KEYMAPSIZE] = {
        [0]      = KEY_5, //for %          //KEY_ESC,               
        [1]      = KEY_9,  //(                      // KEY_BACKSPACE,         
        [2]      = KEY_0,  //)                     // KEY_TAB,             
        [3]      = KEY_8, //*                            //KEY_LEFTSHIFT,                                   // KEY_LEFTSHIFT,        
        [4]      = KEY_2, //@                                  // KEY_LEFTCTRL,         
        [5]      = KEY_LEFTSHIFT,                            // KEY_DUMMY,                 // KEY_DUMMY, //prv KEY_DUMMY
        [6]      = KEY_DUMMY,
        [7]      = KEY_DUMMY,                     
        [8]      = KEY_DUMMY,
        [9]      = KEY_DUMMY,
        [10]     = KEY_MINUS,           //ticket    
        [11]     = KEY_SLASH,           //stage             
        [12]     = KEY_EQUAL, 	        //insp           
        [13]     = KEY_BACKSLASH,       //repeat
        [14]     = KEY_LEFTBRACE,       //datacomm
        [15]     = KEY_BACKSPACE,                // KEY_DUMMY, //free left
        [16]     = KEY_DUMMY, //free left
        [17]     = KEY_DUMMY,//free left
        [18]     = KEY_DUMMY,//free left
        [19]     = KEY_DUMMY,//free left

        [20]     = KEY_COMMA, //lug
        [21]     = KEY_RIGHTSHIFT,//pass 
        [22]     = KEY_1,
        [23]     = KEY_2,
        [24]     = KEY_3,
        [25]     = KEY_DUMMY, //free left
        [26]     = KEY_DUMMY,//free left
        [27]     = KEY_DUMMY,//free left
        [28]     = KEY_DUMMY,//free left
        [29]     = KEY_DUMMY,//free left                     

        [30]     = KEY_APOSTROPHE, //report               
        [31]     = 0x7b,           //concs       
        [32]     = KEY_4,
        [33]     = KEY_5,
        [34]     = KEY_6,
        [35]     = KEY_DUMMY, //free left
        [36]     = KEY_DUMMY, //free left
        [37]     = KEY_DUMMY, //free left
        [38]     = KEY_DUMMY, //free left
        [39]     = KEY_DUMMY, //free left

        [40]     =  ')',     //faretable                            

        [41]     =  KEY_RIGHTBRACE, //credit
        [42]     = KEY_7,
        [43]     = KEY_8,
        [44]     = KEY_9,
        [45]     = KEY_DUMMY,//free left
        [46]     = KEY_DUMMY,//free left
        [47]     = KEY_DUMMY,//free left
        [48]     = KEY_DUMMY,//free left
        [48]     = KEY_DUMMY,//free left
        [49]     = KEY_DUMMY,//free left
        [50]      = KEY_DUMMY, //power                               
        [51]      = KEY_3, //for #                          // KEY_DUMMY, //papfd
        [52]      = KEY_SEMICOLON, //expans   
        [53]      = KEY_0,
        [54]      = KEY_6, //for ^                   //KEY_ENTER,              
        [55]      = KEY_DUMMY, //free left
        [56]      = KEY_DUMMY, //free left
        [57]      = KEY_DUMMY, //free left
        [58]      = KEY_DUMMY, //free left
        [59]      = KEY_DUMMY, //free left

        [60]      = KEY_DOT,               //55,
        [61]      = KEY_1,
        [62]      = KEY_DOT,

        [63]      = KEY_A,               //46,
        [64]      = KEY_B,
        [65]      = KEY_C,

        [66]      = KEY_D,               //46,
        [67]      = KEY_E,
        [68]      = KEY_F,

        [69]      = KEY_G,               //46,
        [70]      = KEY_H,
        [71]      = KEY_I,

        [72]      = KEY_J,               //46,
        [73]      = KEY_K,
        [74]      = KEY_L,
        [75]      = KEY_M,               //46,
        [76]      = KEY_N,
        [77]      = KEY_O,

        [78]      = KEY_P,               //46,
        [79]      = KEY_Q,
        [80]      = KEY_R,

        [81]      = KEY_T,               //46,
        [82]      = KEY_U,
        [83]      = KEY_V,

        [84]      = KEY_W,               //46,
        [85]      = KEY_X,
        [86]      = KEY_Y,

        [87]      = KEY_Z,
        [88]      = KEY_S,
        [89]      = 57,    //KEY_SPACE,   
};

static struct adp5588_kpad_platform_data adp_pdata= {
        .rows           = 6,
        .cols           = 10,
        .keymap         = adp5588_keymap,
        .keymapsize     = ARRAY_SIZE(adp5588_keymap),
        .repeat         = 0,
};


static int my_timer_callback( unsigned long data )
{

        struct timeval curr_tm;
        timer_expired =1;
        do_gettimeofday(&curr_tm);


        //printk( "my_timer_callback called (jiffies = %lld) Current Time == %ld \n", get_jiffies_64(),curr_tm.tv_sec);
        return 0;

}
static int my_timer_callback_bazzer( unsigned long data )
{

	pwm_beeper_key(0);
	return 0;

}



	
/*******************************************************************************************/

static int adp5588_read(struct i2c_client *client, u8 reg)
{
	int ret = i2c_smbus_read_byte_data(client, reg);

	if (ret < 0)
		dev_err(&client->dev, "Read Error\n");

	return ret;
}

static int adp5588_write(struct i2c_client *client, u8 reg, u8 val)
{
	return i2c_smbus_write_byte_data(client, reg, val);
}

#ifdef CONFIG_GPIOLIB
static int adp5588_gpio_get_value(struct gpio_chip *chip, unsigned off)
{
	struct adp5588_kpad *kpad = container_of(chip, struct adp5588_kpad, gc);
	unsigned int bank = ADP5588_BANK(kpad->gpiomap[off]);
	unsigned int bit = ADP5588_BIT(kpad->gpiomap[off]);
	int val;

	mutex_lock(&kpad->gpio_lock);

	if (kpad->dir[bank] & bit)
		val = kpad->dat_out[bank];
	else
		val = adp5588_read(kpad->client, GPIO_DAT_STAT1 + bank);

	mutex_unlock(&kpad->gpio_lock);

	return !!(val & bit);
}

static void adp5588_gpio_set_value(struct gpio_chip *chip,
				   unsigned off, int val)
{
	struct adp5588_kpad *kpad = container_of(chip, struct adp5588_kpad, gc);
	unsigned int bank = ADP5588_BANK(kpad->gpiomap[off]);
	unsigned int bit = ADP5588_BIT(kpad->gpiomap[off]);

	mutex_lock(&kpad->gpio_lock);

	if (val)
		kpad->dat_out[bank] |= bit;
	else
		kpad->dat_out[bank] &= ~bit;

	adp5588_write(kpad->client, GPIO_DAT_OUT1 + bank,
			   kpad->dat_out[bank]);

	mutex_unlock(&kpad->gpio_lock);
}

static int adp5588_gpio_direction_input(struct gpio_chip *chip, unsigned off)
{
	struct adp5588_kpad *kpad = container_of(chip, struct adp5588_kpad, gc);
	unsigned int bank = ADP5588_BANK(kpad->gpiomap[off]);
	unsigned int bit = ADP5588_BIT(kpad->gpiomap[off]);
	int ret;

	mutex_lock(&kpad->gpio_lock);

	kpad->dir[bank] &= ~bit;
	ret = adp5588_write(kpad->client, GPIO_DIR1 + bank, kpad->dir[bank]);

	mutex_unlock(&kpad->gpio_lock);

	return ret;
}

static int adp5588_gpio_direction_output(struct gpio_chip *chip,
					 unsigned off, int val)
{
	struct adp5588_kpad *kpad = container_of(chip, struct adp5588_kpad, gc);
	unsigned int bank = ADP5588_BANK(kpad->gpiomap[off]);
	unsigned int bit = ADP5588_BIT(kpad->gpiomap[off]);
	int ret;

	mutex_lock(&kpad->gpio_lock);

	kpad->dir[bank] |= bit;

	if (val)
		kpad->dat_out[bank] |= bit;
	else
		kpad->dat_out[bank] &= ~bit;

	ret = adp5588_write(kpad->client, GPIO_DAT_OUT1 + bank,
				 kpad->dat_out[bank]);
	ret |= adp5588_write(kpad->client, GPIO_DIR1 + bank,
				 kpad->dir[bank]);

	mutex_unlock(&kpad->gpio_lock);

	return ret;
}

static int adp5588_build_gpiomap(struct adp5588_kpad *kpad,
				const struct adp5588_kpad_platform_data *pdata)
{
	bool pin_used[ADP5588_MAXGPIO];
	int n_unused = 0;
	int i;

	memset(pin_used, 0, sizeof(pin_used));

	for (i = 0; i < pdata->rows; i++)
		pin_used[i] = true;

	for (i = 0; i < pdata->cols; i++)
		pin_used[i + GPI_PIN_COL_BASE - GPI_PIN_BASE] = true;

	for (i = 0; i < kpad->gpimapsize; i++)
		pin_used[kpad->gpimap[i].pin - GPI_PIN_BASE] = true;

	for (i = 0; i < ADP5588_MAXGPIO; i++)
		if (!pin_used[i])
			kpad->gpiomap[n_unused++] = i;

	return n_unused;
}

static int adp5588_gpio_add(struct adp5588_kpad *kpad)
{
	struct device *dev = &kpad->client->dev;
	//commented by arun analogics
	//const struct adp5588_kpad_platform_data *pdata = dev_get_platdata(dev);
	const struct adp5588_kpad_platform_data *pdata = &adp_pdata;
	const struct adp5588_gpio_platform_data *gpio_data = pdata->gpio_data;
	int i, error;

	if (!gpio_data)
		return 0;

	kpad->gc.ngpio = adp5588_build_gpiomap(kpad, pdata);
	if (kpad->gc.ngpio == 0) {
		dev_info(dev, "No unused gpios left to export\n");
		return 0;
	}

	kpad->export_gpio = true;

	kpad->gc.direction_input = adp5588_gpio_direction_input;
	kpad->gc.direction_output = adp5588_gpio_direction_output;
	kpad->gc.get = adp5588_gpio_get_value;
	kpad->gc.set = adp5588_gpio_set_value;
	kpad->gc.can_sleep = 1;

	kpad->gc.base = gpio_data->gpio_start;
	kpad->gc.label = kpad->client->name;
	kpad->gc.owner = THIS_MODULE;
	kpad->gc.names = gpio_data->names;

	mutex_init(&kpad->gpio_lock);

	error = gpiochip_add(&kpad->gc);
	if (error) {
		dev_err(dev, "gpiochip_add failed, err: %d\n", error);
		return error;
	}

	for (i = 0; i <= ADP5588_BANK(ADP5588_MAXGPIO); i++) {
		kpad->dat_out[i] = adp5588_read(kpad->client,
						GPIO_DAT_OUT1 + i);
		kpad->dir[i] = adp5588_read(kpad->client, GPIO_DIR1 + i);
	}

	if (gpio_data->setup) {
		error = gpio_data->setup(kpad->client,
					 kpad->gc.base, kpad->gc.ngpio,
					 gpio_data->context);
		if (error)
			dev_warn(dev, "setup failed, %d\n", error);
	}

	return 0;
}

static void adp5588_gpio_remove(struct adp5588_kpad *kpad)
{
	struct device *dev = &kpad->client->dev;
	//commented by arun analogics
	//const struct adp5588_kpad_platform_data *pdata = dev_get_platdata(dev);
	const struct adp5588_kpad_platform_data *pdata = &adp_pdata;
	const struct adp5588_gpio_platform_data *gpio_data = pdata->gpio_data;
	int error;

	if (!kpad->export_gpio)
		return;

	if (gpio_data->teardown) {
		error = gpio_data->teardown(kpad->client,
					    kpad->gc.base, kpad->gc.ngpio,
					    gpio_data->context);
		if (error)
			dev_warn(dev, "teardown failed %d\n", error);
	}

	gpiochip_remove(&kpad->gc);
}
#else
static inline int adp5588_gpio_add(struct adp5588_kpad *kpad)
{
	return 0;
}

static inline void adp5588_gpio_remove(struct adp5588_kpad *kpad)
{
}
#endif

static void adp5588_report_events(struct adp5588_kpad *kpad, int ev_cnt)
{
	int i, j;

	for (i = 0; i < ev_cnt; i++) {
		int key = adp5588_read(kpad->client, Key_EVENTA + i);
		int key_val = key & KEY_EV_MASK;

		if (key_val >= GPI_PIN_BASE && key_val <= GPI_PIN_END) {
			for (j = 0; j < kpad->gpimapsize; j++) {
				if (key_val == kpad->gpimap[j].pin) {
					input_report_switch(kpad->input,
							kpad->gpimap[j].sw_evt,
							key & KEY_EV_PRESSED);
					break;
				}
			}
		} else {
			input_report_key(kpad->input,
					kpad->keycode[key_val - 1],
					key & KEY_EV_PRESSED);
		}
	}
}
//###################################################################
static int umh_test( void )
{
        struct subprocess_info *sub_info;
        char *argv[] = { "/home/admin/shutdown/run.sh", "help!", NULL };
        static char *envp[] ={"HOME=/","TERM=linux","PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

        sub_info = call_usermodehelper_setup(argv[0], argv, envp, GFP_ATOMIC , NULL , NULL ,NULL);

        if(sub_info == NULL)
        {
                printk("call_usermodehelper failed\n\r");
                return -ENOMEM;
        }
        return call_usermodehelper_exec(sub_info,UMH_NO_WAIT); //prv UMH_WAIT_PROC 
}
//##################################################################



static void adp5588_work(struct work_struct *work)
{
	struct adp5588_kpad *kpad = container_of(work,
			struct adp5588_kpad, work.work);
	struct i2c_client *client = kpad->client;
	int status, ev_cnt;
	int finval, Bcnt, i, key;
	unsigned short endflag = 0;
	// Key Map Table LookUP  First Dimension = Finval  , Second Dimension = Flag; //0 index for flag

	unsigned char lookup_array[64][16] = {

		[22][0]=4,[22][1]=22,[22][2]=22,[22][3]=60,[22][4]=60,[22][5]=22,[22][6]=22,[22][7]=22, [22][8]=22, //for keys 1.

		[23][0]=8,[23][1]=23,[23][2]=23,[23][3]=63,[23][4]=63,[23][5]=64,[23][6]=64,[23][7]=65, [23][8]=65, //for keys 2abc

		[24][0]=8,[24][1]=24,[24][2]=24,[24][3]=66,[24][4]=66,[24][5]=67,[24][6]=67,[24][7]=68, [24][8]=68,//for keys 3def

		[32][0]=8,[32][1]=32,[32][2]=32,[32][3]=69,[32][4]=69,[32][5]=70,[32][6]=70,[32][7]=71, [32][8]=71,//for keys 4ghi

		[33][0]=8,[33][1]=33,[33][2]=33,[33][3]=72,[33][4]=72,[33][5]=73,[33][6]=73,[33][7]=74, [33][8]=74,//for keys 5jkl

		[34][0]=8,[34][1]=34,[34][2]=34,[34][3]=75,[34][4]=75,[34][5]=76,[34][6]=76,[34][7]=77, [34][8]=77,//for 6mno

		[42][0]=10,[42][1]=42,[42][2]=42,[42][3]=78,[42][4]=78,[42][5]=79,[42][6]=79,[42][7]=80,[42][8]=80,[42][9]=88,[42][10]=88, //for keys 7pqrs

		[43][0]=8, [43][1]=43,[43][2]=43,[43][3]=81,[43][4]=81,[43][5]=82,[43][6]=82,[43][7]=83, [43][8]=83, //for keys 8tuv

		[44][0]=10,[44][1]=44,[44][2]=44,[44][3]=84,[44][4]=84,[44][5]=85,[44][6]=85,[44][7]=86,[44][8]=86,[44][9]=87, [44][10]=87, //for keys 9wxyz

		[53][0]=4,[53][1]=53,[53][2]=53,[53][3]=89,[53][4]=89,[53][5]=53,[53][6]=89,[53][7]=89, [53][8]=89, //for 0 space

	};

	status = adp5588_read(client, INT_STAT);

	if (status & ADP5588_OVR_FLOW_INT)	/* Unlikely and should never happen */
		dev_err(&client->dev, "Event Overflow Error\n");

	if (status & ADP5588_KE_INT) {
		ev_cnt = adp5588_read(client, KEY_LCK_EC_STAT) & ADP5588_KEC;
		if (ev_cnt) {

			//				umh_test();

			// for (i = 0; i < ev_cnt; i++) {
			key = adp5588_read(client, Key_EVENTA + i);
			del_timer(&my_timer);
			mod_timer( &my_timer, get_jiffies_64() + msecs_to_jiffies(timerset) ); //1sec delay for 1sec delay i.e prv 1000
			//printk("\nkey == %d \n",(key & KEY_EV_MASK));
			finval = (key & KEY_EV_MASK) -1;

			if(key & KEY_EV_PRESSED)
			{
				del_timer(&my_timer_bazzer);
				pwm_beeper_key(2000000);	
				mod_timer( &my_timer_bazzer, get_jiffies_64() + msecs_to_jiffies(100) ); //1sec delay for 1sec delay i.e prv 1000

			}
			del_timer(&my_timer);
			mod_timer( &my_timer, get_jiffies_64() + msecs_to_jiffies(timerset) );//1sec delay //pr 1000
			//printk("finval====%d\n",finval);

			if((finval == 22)||(finval == 23)||(finval == 24)||
					(finval == 32)||(finval == 33)||(finval == 34)||
					(finval == 42)||(finval == 43)||(finval == 44)||(finval == 53))
			{
				endflag = lookup_array[finval][0];
				flag++; 				
				if(timer_expired || prev_finvalue != finval) {
					//printk("Timer Expired Repeat Old Value = 2 \n");
					flag = 1;
				}  else { 
					// input_report_key(kpad->input,kpad->keycode[1],key & KEY_EV_PRESSED);   for bksp
					input_report_key(kpad->input,kpad->keycode[15],key & KEY_EV_PRESSED);  
				}
				if ((flag>=1) && (flag <= endflag)) {

					//umh_test();
					//printk("Look Up Array = %d \n",lookup_array[finval][flag]);
					input_report_key(kpad->input,kpad->keycode[lookup_array[finval][flag]],key & KEY_EV_PRESSED);
				}
				if (flag % endflag == 0)
					flag=0;                                                          
				timer_expired = 0;
			}
			else if(finval == 5)                   //added for shifttab prv 3
			{
				//printk("iam in leftshift\n");  
				input_report_key(kpad->input,kpad->keycode[finval],key & KEY_EV_PRESSED); //leftshift 
				input_report_key(kpad->input,kpad->keycode[2],key & KEY_EV_PRESSED);      // tab  
			}
			else if((finval == 0)||(finval == 1)||(finval == 2)||(finval == 4)||(finval == 3)||(finval == 51)||(finval == 54))
			{
				input_report_key(kpad->input,kpad->keycode[5],key & KEY_EV_PRESSED); //prv 3
				input_report_key(kpad->input,kpad->keycode[finval],key & KEY_EV_PRESSED);}

			/****************added for special characters********/
			else if((finval == 21) || (finval == 31))
			{
				input_report_key(kpad->input,kpad->keycode[5],key & KEY_EV_PRESSED); //prv 5
				input_event(kpad->input,EV_KEY,kpad->keycode[finval+1],key & KEY_EV_PRESSED);
			}
			else
				input_report_key(kpad->input,kpad->keycode[finval],key & KEY_EV_PRESSED); //added raghuram

			prev_finvalue = finval;		    
			//                                printk("\nKey code Sent");
			//    printk(" \n Final Map Value = %c \n ",kpad->keycode[(key & KEY_EV_MASK) - 1]);
			//         }
			input_sync(kpad->input);

		}

	}
	adp5588_write(client, INT_STAT, status); /* Status is W1C */
}


static irqreturn_t adp5588_irq(int irq, void *handle)
{
	struct adp5588_kpad *kpad = handle;

	/*
	 * use keventd context to read the event fifo registers
	 * Schedule readout at least 25ms after notification for
	 * REVID < 4
	 */

	schedule_delayed_work(&kpad->work, kpad->delay);

	return IRQ_HANDLED;
}

static int adp5588_setup(struct i2c_client *client)
{
	//commented by arun analogics
	//const struct adp5588_kpad_platform_data *pdata =
	//		dev_get_platdata(&client->dev);
	
	const struct adp5588_kpad_platform_data *pdata = &adp_pdata;
	const struct adp5588_gpio_platform_data *gpio_data = pdata->gpio_data;
	int i, ret;
	unsigned char evt_mode1 = 0, evt_mode2 = 0, evt_mode3 = 0;

	ret = adp5588_write(client, KP_GPIO1, KP_SEL(pdata->rows));
	ret |= adp5588_write(client, KP_GPIO2, KP_SEL(pdata->cols) & 0xFF);
	ret |= adp5588_write(client, KP_GPIO3, KP_SEL(pdata->cols) >> 8);

	if (pdata->en_keylock) {
		ret |= adp5588_write(client, UNLOCK1, pdata->unlock_key1);
		ret |= adp5588_write(client, UNLOCK2, pdata->unlock_key2);
		ret |= adp5588_write(client, KEY_LCK_EC_STAT, ADP5588_K_LCK_EN);
	}

	for (i = 0; i < KEYP_MAX_EVENT; i++)
		ret |= adp5588_read(client, Key_EVENTA);

	for (i = 0; i < pdata->gpimapsize; i++) {
		unsigned short pin = pdata->gpimap[i].pin;

		if (pin <= GPI_PIN_ROW_END) {
			evt_mode1 |= (1 << (pin - GPI_PIN_ROW_BASE));
		} else {
			evt_mode2 |= ((1 << (pin - GPI_PIN_COL_BASE)) & 0xFF);
			evt_mode3 |= ((1 << (pin - GPI_PIN_COL_BASE)) >> 8);
		}
	}

	if (pdata->gpimapsize) {
		ret |= adp5588_write(client, GPI_EM1, evt_mode1);
		ret |= adp5588_write(client, GPI_EM2, evt_mode2);
		ret |= adp5588_write(client, GPI_EM3, evt_mode3);
	}

	if (gpio_data) {
		for (i = 0; i <= ADP5588_BANK(ADP5588_MAXGPIO); i++) {
			int pull_mask = gpio_data->pullup_dis_mask;

			ret |= adp5588_write(client, GPIO_PULL1 + i,
				(pull_mask >> (8 * i)) & 0xFF);
		}
	}

	ret |= adp5588_write(client, INT_STAT,
				ADP5588_CMP2_INT | ADP5588_CMP1_INT |
				ADP5588_OVR_FLOW_INT | ADP5588_K_LCK_INT |
				ADP5588_GPI_INT | ADP5588_KE_INT); /* Status is W1C */

	ret |= adp5588_write(client, CFG, ADP5588_INT_CFG |
					  ADP5588_OVR_FLOW_IEN |
					  ADP5588_KE_IEN);

	if (ret < 0) {
		dev_err(&client->dev, "Write Error\n");
		return ret;
	}

	return 0;
}

static void adp5588_report_switch_state(struct adp5588_kpad *kpad)
{
	int gpi_stat1 = adp5588_read(kpad->client, GPIO_DAT_STAT1);
	int gpi_stat2 = adp5588_read(kpad->client, GPIO_DAT_STAT2);
	int gpi_stat3 = adp5588_read(kpad->client, GPIO_DAT_STAT3);
	int gpi_stat_tmp, pin_loc;
	int i;

	for (i = 0; i < kpad->gpimapsize; i++) {
		unsigned short pin = kpad->gpimap[i].pin;

		if (pin <= GPI_PIN_ROW_END) {
			gpi_stat_tmp = gpi_stat1;
			pin_loc = pin - GPI_PIN_ROW_BASE;
		} else if ((pin - GPI_PIN_COL_BASE) < 8) {
			gpi_stat_tmp = gpi_stat2;
			pin_loc = pin - GPI_PIN_COL_BASE;
		} else {
			gpi_stat_tmp = gpi_stat3;
			pin_loc = pin - GPI_PIN_COL_BASE - 8;
		}

		if (gpi_stat_tmp < 0) {
			dev_err(&kpad->client->dev,
				"Can't read GPIO_DAT_STAT switch %d default to OFF\n",
				pin);
			gpi_stat_tmp = 0;
		}

		input_report_switch(kpad->input,
				    kpad->gpimap[i].sw_evt,
				    !(gpi_stat_tmp & (1 << pin_loc)));
	}

	input_sync(kpad->input);
}

//matrix keymap added by arun analogics
int adp5588_parse_of_params(struct device *dev,
				  unsigned int *rows, unsigned int *cols)
{
	struct device_node *np = dev->of_node;

	if (!np) {
		dev_err(dev, "missing DT data");
		return -EINVAL;
	}
	of_property_read_u32(np, "keypad,num-rows", rows);
	of_property_read_u32(np, "keypad,num-columns", cols);
	if (!*rows || !*cols) {
		dev_err(dev, "number of keypad rows/columns not specified\n");
		return -EINVAL;
	}

	return 0;
}
/*################################*/
static  long keybdev_user_ioctl(struct file *file,signed int cmd, unsigned long arg)

{
       unsigned int num_key;
       num_key=arg;
//      printk("iam in kernel ioctl\n");     
//      printk("num_key========%d\n",num_key); 
     if(num_key==ON){
                        
           timerset=MIN_TIMER;
//           printk("timerset===%d\n",timerset); 
           }            
     else 
           timerset=MAX_TIMER;
        return 0;
};

static const struct file_operations keybdev_user_keyb_file_ops = {
        .owner           = THIS_MODULE,
          .unlocked_ioctl = keybdev_user_ioctl,
};

static struct miscdevice keybdev = {
        8,
        "keybdev",
        &keybdev_user_keyb_file_ops,
};
/*############################3#33*/

static int adp5588_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct adp5588_kpad *kpad;
	//add by rakesh analogics
	setup_timer( &my_timer, my_timer_callback, 0 );
        setup_timer( &my_timer_bazzer, my_timer_callback_bazzer, 0 );	
	misc_register(&keybdev);
	//commented by arun analogics
	//struct adp5588_kpad_platform_data *pdata =
	//		dev_get_platdata(&client->dev);
	struct adp5588_kpad_platform_data *pdata = &adp_pdata;
	struct input_dev *input;
	unsigned int revid;
	int ret, i;
	int error;
        struct pwm_beeper *beeper;

	//BUZZER_EXPORT();

	if (!i2c_check_functionality(client->adapter,
					I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev, "SMBUS Byte Data not Supported\n");
		return -EIO;
	}

	//matrix keymap added by arun analogics
	//error = adp5588_parse_of_params(dev, &(pdata->rows), &(pdata->cols));
	//error = adp5588_parse_of_params(dev, &rows, &cols);
	//if (error)
	//	return error;
	
	//printk("****** rows : %d cols : %d ************\n", rows, cols);

	if (!pdata) {
		dev_err(&client->dev, "no platform data?\n");
		return -EINVAL;
	}

	if (!pdata->rows || !pdata->cols || !pdata->keymap) {
		dev_err(&client->dev, "no rows, cols or keymap from pdata\n");
		return -EINVAL;
	}

	if (pdata->keymapsize != ADP5588_KEYMAPSIZE) {
		dev_err(&client->dev, "invalid keymapsize\n");
		return -EINVAL;
	}

	if (!pdata->gpimap && pdata->gpimapsize) {
		dev_err(&client->dev, "invalid gpimap from pdata\n");
		return -EINVAL;
	}

	if (pdata->gpimapsize > ADP5588_GPIMAPSIZE_MAX) {
		dev_err(&client->dev, "invalid gpimapsize\n");
		return -EINVAL;
	}

	//###########################################################
//	struct pwm_beeper *beeper;
//	int error;

	beeper = kzalloc(sizeof(*beeper), GFP_KERNEL);
	if (!beeper)
		return -ENOMEM;

	beeper->pwm = pwm_get(&client->dev, NULL);
	if (IS_ERR(beeper->pwm)) {
		dev_dbg(&client->dev, "unable to request PWM, trying legacy API\n");
		//beeper->pwm = pwm_request(pwm_id, "pwm beeper");
	}

	if (IS_ERR(beeper->pwm)) {
		error = PTR_ERR(beeper->pwm);
		//dev_err(&pdev->dev, "Failed to request pwm device: %d\n", error);
		goto err_free_mem;
	}
	        g_beeper=beeper; //rakesh analogics

//###########################################################



	for (i = 0; i < pdata->gpimapsize; i++) {
		unsigned short pin = pdata->gpimap[i].pin;

		if (pin < GPI_PIN_BASE || pin > GPI_PIN_END) {
			dev_err(&client->dev, "invalid gpi pin data\n");
			return -EINVAL;
		}

		if (pin <= GPI_PIN_ROW_END) {
			if (pin - GPI_PIN_ROW_BASE + 1 <= pdata->rows) {
				dev_err(&client->dev, "invalid gpi row data\n");
				return -EINVAL;
			}
		} else {
			if (pin - GPI_PIN_COL_BASE + 1 <= pdata->cols) {
				dev_err(&client->dev, "invalid gpi col data\n");
				return -EINVAL;
			}
		}
	}

	if (!client->irq) {
		dev_err(&client->dev, "no IRQ?\n");
		return -EINVAL;
	}

	kpad = kzalloc(sizeof(*kpad), GFP_KERNEL);
	input = input_allocate_device();
	if (!kpad || !input) {
		error = -ENOMEM;
		goto err_free_mem;
	}

	kpad->client = client;
	kpad->input = input;
	INIT_DELAYED_WORK(&kpad->work, adp5588_work);

	ret = adp5588_read(client, DEV_ID);
	if (ret < 0) {
		error = ret;
		goto err_free_mem;
	}

	revid = (u8) ret & ADP5588_DEVICE_ID_MASK;
	if (WA_DELAYED_READOUT_REVID(revid))
		kpad->delay = msecs_to_jiffies(30);

	input->name = client->name;
	input->phys = "adp5588-keys/input0";
	input->dev.parent = &client->dev;

	input_set_drvdata(input, kpad);

	input->id.bustype = BUS_I2C;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = revid;

	input->keycodesize = sizeof(kpad->keycode[0]);
	input->keycodemax = pdata->keymapsize;
	input->keycode = kpad->keycode;

	memcpy(kpad->keycode, pdata->keymap,
		pdata->keymapsize * input->keycodesize);

	kpad->gpimap = pdata->gpimap;
	kpad->gpimapsize = pdata->gpimapsize;

	/* setup input device */
	__set_bit(EV_KEY, input->evbit);

	if (pdata->repeat)
		__set_bit(EV_REP, input->evbit);

	for (i = 0; i < input->keycodemax; i++)
		if (kpad->keycode[i] <= KEY_MAX)
			__set_bit(kpad->keycode[i], input->keybit);
	__clear_bit(KEY_RESERVED, input->keybit);

	if (kpad->gpimapsize)
		__set_bit(EV_SW, input->evbit);
	for (i = 0; i < kpad->gpimapsize; i++)
		__set_bit(kpad->gpimap[i].sw_evt, input->swbit);

	error = input_register_device(input);
	if (error) {
		dev_err(&client->dev, "unable to register input device\n");
		goto err_free_mem;
	}

	error = request_irq(client->irq, adp5588_irq,
			    IRQF_TRIGGER_FALLING,
			    client->dev.driver->name, kpad);
	if (error) {
		dev_err(&client->dev, "irq %d busy?\n", client->irq);
		goto err_unreg_dev;
	}

	error = adp5588_setup(client);
	if (error)
		goto err_free_irq;

	if (kpad->gpimapsize)
		adp5588_report_switch_state(kpad);

	error = adp5588_gpio_add(kpad);
	if (error)
		goto err_free_irq;

	device_init_wakeup(&client->dev, 1);
	i2c_set_clientdata(client, kpad);

	dev_info(&client->dev, "Rev.%d keypad, irq %d\n", revid, client->irq);

	//Buzzer GPIO added by arun analogics
//	Gpio_set_value(BUZZER, "buzzer", 0, 0);  //ashok
	/*while(1)
	{
		Set(BUZZER,1);
		udelay(1000);
		Set(BUZZER,0);
		udelay(1000);

	}*/
	
	return 0;

 err_free_irq:
	free_irq(client->irq, kpad);
	cancel_delayed_work_sync(&kpad->work);
 err_unreg_dev:
	input_unregister_device(input);
	input = NULL;
 err_free_mem:
	input_free_device(input);
	kfree(kpad);

	return error;
}

static int adp5588_remove(struct i2c_client *client)
{
	struct adp5588_kpad *kpad = i2c_get_clientdata(client);

	adp5588_write(client, CFG, 0);
	free_irq(client->irq, kpad);
	cancel_delayed_work_sync(&kpad->work);
	input_unregister_device(kpad->input);
	adp5588_gpio_remove(kpad);
	kfree(kpad);

	return 0;
}

#ifdef CONFIG_PM
static int adp5588_suspend(struct device *dev)
{
	struct adp5588_kpad *kpad = dev_get_drvdata(dev);
	struct i2c_client *client = kpad->client;

	disable_irq(client->irq);
	cancel_delayed_work_sync(&kpad->work);

	if (device_may_wakeup(&client->dev))
		enable_irq_wake(client->irq);

	return 0;
}

static int adp5588_resume(struct device *dev)
{
	struct adp5588_kpad *kpad = dev_get_drvdata(dev);
	struct i2c_client *client = kpad->client;

	if (device_may_wakeup(&client->dev))
		disable_irq_wake(client->irq);

	enable_irq(client->irq);

	return 0;
}

static const struct dev_pm_ops adp5588_dev_pm_ops = {
	.suspend = adp5588_suspend,
	.resume  = adp5588_resume,
};
#endif
#if 0
static  long keybdev_user_ioctl(struct file *file,signed int cmd, unsigned long arg)

{
       unsigned int num_key;
       num_key=arg;
//      printk("iam in kernel ioctl\n");     
//      printk("num_key========%d\n",num_key); 
     if(num_key==ON){
                        
           timerset=MIN_TIMER;
//           printk("timerset===%d\n",timerset); 
           }            
     else 
           timerset=MAX_TIMER;
        return 0;
};

static const struct file_operations keybdev_user_keyb_file_ops = {
        .owner           = THIS_MODULE,
          .unlocked_ioctl = keybdev_user_ioctl,
};

static struct miscdevice keybdev = {
        8,
        "keybdev",
        &keybdev_user_keyb_file_ops,
};

#endif

static const struct i2c_device_id adp5588_id[] = {
	{ "adp5588-keys", 0 },
	{ "adp5587-keys", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, adp5588_id);

//table id added by arun analogics
#ifdef CONFIG_OF

static const struct of_device_id adp5588_match_table[] = {
		{.compatible = "adp5588-keys", },
		{ },
};
#endif

static struct i2c_driver adp5588_driver = {
	.driver = {
		.name = KBUILD_MODNAME,
		//table id added by arun analogics
		.of_match_table = of_match_ptr(adp5588_match_table),
#ifdef CONFIG_PM
		.pm   = &adp5588_dev_pm_ops,
#endif
	},
	.probe    = adp5588_probe,
	.remove   = adp5588_remove,
	.id_table = adp5588_id,
};

module_i2c_driver(adp5588_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hennerich <hennerich@blackfin.uclinux.org>");
MODULE_DESCRIPTION("ADP5588/87 Keypad driver");
