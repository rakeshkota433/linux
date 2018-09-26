//***********************************************************************//
//  This is for 2I-impact for terrain-335x-board//
//  *********************************************************************//

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
#include <asm/delay.h>
#include <asm/pmu.h>
#include "Am335x_gpio.h"
//#include "../../drivers/staging/iio/iio.h"


#define ON 1
#define OFF 0


//2I-PRINTER STROBES
#define STB0    GPIO_TO_PIN(1, 7) //GPMC_ALE FOR 2t to 2I //GPMC_AD7 GPIO MOD7 in BANK1 of 7ST pin
#define STB1    GPIO_TO_PIN(1, 1) //GPMC_AD1  select as GPIO MOD7 in BANK1 of 1ST pin 
#define STB2    GPIO_TO_PIN(1, 2) //GPMC_AD2  select as GPIO MOD7 in BANK1 of 2ND pin 
#define STB3    GPIO_TO_PIN(1, 3) //GPMC_AD3  select as GPIO MOD7 in BANK1 of 3RD pin 
#define STB4    GPIO_TO_PIN(1, 4) //GPMC_AD4  select as GPIO MOD7 in BANK1 of 4th pin 
#define STB5    GPIO_TO_PIN(1, 5) //GPMC_AD5  select as GPIO MOD7 in BANK1 of 5th pin 
#define STB6    GPIO_TO_PIN(1, 6) //GPMC_AD6  select as GPIO MOD7 in BANK1 of 6th pin 
#define STB7    GPIO_TO_PIN(1, 20) //GPMC_A4  select as GPIO MOD7 in BANK1 of 20th pin 

//STEPPER MOTOR PINS  
#define MOT_PWR  GPIO_TO_PIN(1, 22) //GPMC_A6  select as GPIO MOD7 in BANK1 of 25th pin 

#define RSTPIN  GPIO_TO_PIN(1, 23) //GPMC_A7 select as GPIO MOD7 in BANK1 of 26th input pin 
#define TIMCNT  GPIO_TO_PIN(1, 16) //GPMC_A0 select as GPIO MOD7 in BANK1 of 6th input pin 


////PRINTER CONTROL PINS
#define PRN_PFO GPIO_TO_PIN(1, 21) //GPMC_A5 select as GPIO MOD7 in BANK1 of 24th pin
#define PRN_PWR GPIO_TO_PIN(1, 24) //GPMC_A11 select as GPIO MOD7 in BANK1 of 27th pin

#define Set(X,V) gpio_set_value(X,V)


#define CS_CHANGE(Pxfer)        ((Pxfer).cs_change = 0)

#define Pwidth  48



/*
#define PFD             AT91_PIN_PC6
#define PRN_PWR        	AT91_PIN_PC4	
#define STB0            AT91_PIN_PA20	
#define STB1            AT91_PIN_PA21
#define STB2            AT91_PIN_PA22
#define STB3            AT91_PIN_PA23
#define STB4            AT91_PIN_PA24
#define STB5            AT91_PIN_PA25
#define STB6            AT91_PIN_PA26
#define STB7            AT91_PIN_PA27
#define MOT_PWR         AT91_PIN_PC5
#define TIMCNT          AT91_PIN_PA5
#define RSTPIN          AT91_PIN_PB0

#define  PFD_ON    at91_set_gpio_value(PFD,0)             
#define  PFD_OFF   at91_set_gpio_value(PFD,1)
*/

					
#define IMGSIZE 2097152
#define NULL_TERM '\0'

/***********************************Look Up Table ***************************************************************/


unsigned char const line_10[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

//SPECIAL CHARACTERS
0x00,0x08,0x14,0x14,0x08,0x30,0x18,0x18,0x04,0x10,0x00,0x00,0x00,0x00,0x00,
0x00,

//NUMERIC DIGITS WITH SPECIAL CHARACTERS
0x1c,0x08,0x1c,0x3e,0x04,0x3e,0x0c,0x3e,0x1c,0x1c,0x00,0x00,0x04,0x00,0x10,
0x1c,0x00,

//CAPITAL ALPHABETS WITH SPECIAL CHARACTERS
0x1C,0x3C,0x1C,0x38,0x3E,0x3E,0x1C,0x22,0x1C,0x0E,0x22,0x20,0x22,0x22,0x1C,
0x3C,0x1C,0x3C,0x1E,0x3E,0x22,0x22,0x22,0x22,0x22,0x3E,0x1C,0x00,0x1C,0x08,
0x00,0x00,

//SMALL ALPHABETS WITH SPECIAL CHARACTERS
0x00,0x20,0x00,0x02,0x00,0x0C,0x00,0x20,0x08,0x04,0x20,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x08,0x10};

unsigned char const line_20[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x14,0x14,0x1E,0x32,0x24,
0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x00,0x02,0x22,0x18,0x22,0x04,0x0c,0x20,
0x10,0x02,0x22,0x22,0x18,0x18,0x08,0x00,0x08,0x22,0x00,0x22,0x22,0x22,0x24,
0x20,0x20,0x22,0x22,0x08,0x04,0x24,0x20,0x36,0x22,0x22,0x22,0x22,0x22,0x20,
0x08,0x22,0x22,0x22,0x22,0x22,0x02,0x10,0x00,0x04,0x14,0x00,0x00,0x00,0x20,
0x00,0x02,0x00,0x12,0x1E,0x20,0x00,0x00,0x20,0x08,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x08};

unsigned char const line_30[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x14,0x3E,0x28,0x04,0x28,
0x10,0x10,0x04,0x2A,0x08,0x00,0x00,0x00,0x04,0x26,0x08,0x02,0x08,0x14,0x3c,
0x20,0x04,0x22,0x22,0x18,0x18,0x10,0x3e,0x04,0x02,0x00,0x22,0x22,0x20,0x22,
0x20,0x20,0x20,0x22,0x08,0x04,0x28,0x20,0x2A,0x32,0x22,0x22,0x22,0x22,0x20,
0x08,0x22,0x22,0x22,0x14,0x22,0x04,0x10,0x00,0x04,0x22,0x00,0x00,0x1C,0x2C,
0x1C,0x1A,0x1C,0x10,0x22,0x2C,0x18,0x0C,0x24,0x08,0x34,0x2C,0x1C,0x3C,0x1A,
0x2C,0x1C,0x38,0x22,0x22,0x22,0x22,0x22,0x3E,0x08,0x08,0x08};

unsigned char const line_40[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x14,0x1C,0x08,0x10,
0x00,0x10,0x04,0x1C,0x3E,0x00,0x3E,0x00,0x08,0x2a,0x08,0x04,0x04,0x24,0x02,
0x3c,0x08,0x1c,0x1e,0x00,0x00,0x20,0x00,0x02,0x04,0x00,0x22,0x3C,0x20,0x22,
0x3C,0x3C,0x2E,0x3E,0x08,0x04,0x30,0x20,0x2A,0x2A,0x22,0x3C,0x22,0x3C,0x1C,
0x08,0x22,0x22,0x2A,0x08,0x14,0x10,0x00,0x04,0x00,0x00,0x00,0x00,0x02,0x32,
0x20,0x26,0x22,0x38,0x22,0x32,0x08,0x04,0x28,0x08,0x2A,0x32,0x22,0x22,0x26,
0x32,0x20,0x10,0x22,0x22,0x22,0x14,0x22,0x04,0x10,0x08,0x04};

unsigned char const line_50[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x0A,0x10,0x2A,
0x00,0x10,0x04,0x2A,0x08,0x18,0x00,0x00,0x10,0x32,0x08,0x08,0x02,0x3e,0x02,
0x22,0x10,0x22,0x02,0x18,0x18,0x10,0x3e,0x04,0x08,0x00,0x3E,0x22,0x20,0x22,
0x20,0x20,0x22,0x22,0x08,0x04,0x28,0x20,0x22,0x26,0x22,0x20,0x2A,0x28,0x02,
0x08,0x22,0x22,0x2A,0x14,0x08,0x10,0x10,0x00,0x04,0x00,0x00,0x00,0x1E,0x22,
0x20,0x22,0x3E,0x10,0x1E,0x22,0x08,0x04,0x30,0x08,0x2A,0x22,0x22,0x3C,0x1E,
0x20,0x1C,0x10,0x22,0x22,0x2A,0x08,0x1E,0x08,0x08,0x08,0x08};


unsigned char const line_60[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x3C,0x26,0x24,
0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x18,0x20,0x22,0x08,0x10,0x22,0x04,0x22,
0x22,0x10,0x22,0x04,0x18,0x08,0x08,0x00,0x08,0x00,0x00,0x22,0x22,0x22,0x24,
0x20,0x20,0x22,0x22,0x08,0x24,0x24,0x20,0x22,0x22,0x22,0x20,0x24,0x24,0x02,
0x08,0x22,0x14,0x2A,0x22,0x08,0x20,0x10,0x00,0x04,0x00,0x00,0x00,0x22,0x22,
0x22,0x22,0x20,0x10,0x02,0x22,0x08,0x24,0x28,0x08,0x22,0x22,0x22,0x20,0x02,
0x20,0x02,0x12,0x26,0x14,0x2A,0x14,0x02,0x10,0x08,0x08,0x08};

unsigned char const line_70[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x14,0x08,0x06,0x1A,
0x00,0x04,0x10,0x00,0x00,0x10,0x00,0x18,0x00,0x1c,0x1c,0x3e,0x1c,0x04,0x1c,
0x1c,0x10,0x1c,0x18,0x00,0x10,0x04,0x00,0x10,0x08,0x00,0x22,0x3C,0x1C,0x38,
0x3E,0x20,0x1E,0x22,0x1C,0x18,0x22,0x3E,0x22,0x22,0x1C,0x20,0x1A,0x22,0x3C,
0x08,0x1C,0x08,0x14,0x22,0x08,0x3E,0x1C,0x00,0x1C,0x00,0x3E,0x02,0x1E,0x3C,
0x1c,0x1e,0x1c,0x10,0x1c,0x22,0x1c,0x18,0x24,0x1c,0x22,0x22,0x1c,0x20,0x02,
0x20,0x3c,0x0c,0x1a,0x08,0x14,0x22,0x1c,0x3e,0x04,0x08,0x10};

/*********************************Look Up Table **************************************************************/

static wait_queue_head_t prin_queue;
spinlock_t *Prnblock;
int weventp;
int lcnt=0;
static int powerflag=0;
unsigned int   bit_1,bit_2,bit_3,bit_4,bit_5,bit_6,bit_7,bit_8;
unsigned int prt_dh_flag,prt_dw_flag;
unsigned char test_msg[24],smprn_i=0;
unsigned char  a_char,b_char,c_char,d_char,e_char,f_char,g_char,h_char;
unsigned char  a_dot,b_dot,c_dot,d_dot,e_dot,f_dot,g_dot,h_dot;
unsigned char res_det,time_det,temp;
unsigned char temp_buff[IMGSIZE];
unsigned char const *line_no;


/************************function prototypes*****************************/


void PrintPower(unsigned char Status);
void p_feed(unsigned char Status);
void dot_ln(void);
void dot_1(void);
void st_prt(void);
//void sm_prt(unsigned char);
//void prt_msg(char * prt_msg_data,unsigned char max_char);
void paper_feed(void);
//void logo_prt(void);



void chk(void);
void hi_pul(void);
void lo_pul(void);

void strobes_off(void);
void strobes_on(void);
void strobes_dots(int temp1);


void adg_dots(void);
void beh_dots(void);
void cf_dots(void);


void adg_bits(void);
void beh_bits(void);
void cf_bits(void);

void adglogo_bits(void);
void behlogo_bits(void);
void cflogo_bits(void);

int ac,bc,cc,dc,ec,fc,gc,hc;
/**************************************************************************/

void paper_feed(void)
{
	unsigned char chk_cnt,chk_cnt1;
	PrintPower(ON);
	udelay(1000);

	for(chk_cnt1=0;chk_cnt1<8;chk_cnt1++)
  	{
		while(!gpio_get_value(RSTPIN));

        	for(chk_cnt=0;chk_cnt<6;chk_cnt++)
        	{
                	hi_pul();
                	lo_pul();
        	}

	 	p_feed(OFF);       //enable

    		for(chk_cnt=0;chk_cnt<6;chk_cnt++)
    		{
        		hi_pul();
        		lo_pul();
    		}

   	 	p_feed(ON);      //disable

 	}
      	//udelay(6000);
    	PrintPower(OFF);

}

EXPORT_SYMBOL(paper_feed);

void st_prt(void)
{
	unsigned char chk_cnt;
    	PrintPower(ON);
	
	udelay(1000);
    	chk();
    	line_no = line_10;
    	dot_ln();
	udelay(1000);
        chk();
        line_no =  line_20;
        dot_ln();
   	udelay(1000);
	chk();
    	line_no =  line_30;
        dot_ln();
	udelay(1000);
        chk();
    	line_no = line_40;
        dot_ln();
   	udelay(1000);
        chk();
   	line_no = line_50;
        dot_ln();
  	udelay(1000);
        chk();
        line_no = line_60;
        dot_ln();
   	 udelay(1000);
        chk();
        line_no = line_70;
	dot_ln();
  	udelay(1000);

 	while(!gpio_get_value(RSTPIN));
	for(chk_cnt=0;chk_cnt<6;chk_cnt++)
    	{
        	hi_pul();
        	lo_pul();
    	}

    	p_feed(OFF);       //enable
    
    	for(chk_cnt=0;chk_cnt<6;chk_cnt++)
    	{
        	hi_pul();
        	lo_pul();
    	}

    	p_feed(ON);      //disable

 		
	udelay(1000);
	udelay(1000);
	udelay(1000);
			
   	PrintPower(OFF);
}



/*void logo_prt(void)
{

	int i;	
	unsigned char chk_cnt;

	PrintPower(ON);

	udelay(1000);
	
	//memset(test_msg,0x00,18);

	chk();
		for(i=0;i<16;i++,lcnt++)		
		test_msg[i] = prnlogo_6[lcnt];
	dot_ln();

        udelay(1000);

        chk();
		for(i=0;i<16;i++,lcnt++)
        	test_msg[i] = prnlogo_6[lcnt];
        dot_ln();

        udelay(1000);
        chk();
		for(i=0;i<16;i++,lcnt++)
        	test_msg[i] = prnlogo_6[lcnt];

        dot_ln();

        udelay(1000);
        chk();
        	for(i=0;i<16;i++,lcnt++)
        	test_msg[i] = prnlogo_6[lcnt];

        dot_ln();

        udelay(1000);
        chk();
        	for(i=0;i<16;i++,lcnt++)
        	test_msg[i] = prnlogo_6[lcnt];
        dot_ln();

        udelay(1000);
        chk();
		for(i=0;i<16;i++,lcnt++)
        	test_msg[i] = prnlogo_6[lcnt];
        dot_ln();

        udelay(1000);
        chk();
  		for(i=0;i<16;i++,lcnt++)
        	test_msg[i] = prnlogo_6[lcnt];
        dot_ln();

        udelay(1000);

     //   while(!at91_get_gpio_value(RSTPIN));



   // for(chk_cnt=0;chk_cnt<6;chk_cnt++)
   // {
   //     hi_pul();
   //     lo_pul();
   // }



//udelay(1000);
//udelay(1000);
//udelay(1000);


//   PrintPower(OFF);

}*/






void p_feed(unsigned char Status)
{
        if(Status) 
		Set(PRN_PFO,1);//PFD_OFF;
        else 
		Set(PRN_PFO,0);//PFD_ON;
}




void dot_ln(void)
{
    unsigned char i;

        ac=1,bc=1,cc=1,dc=1,ec=1,fc=1,gc=1,hc=1;

        a_char = 0x00;
        b_char = 0x02;
        c_char = 0x04;
	d_char = 0x06;
        e_char = 0x09;
        f_char = 0x0B;
        g_char = 0x0D;
        h_char = 0x0F;
    
        adglogo_bits();
        behlogo_bits();
        cflogo_bits();
        for(i=0;i<18;i++)
        {
            dot_1();    //1 dot will be printed
 	}
}



void  adglogo_bits(void)
{
    unsigned char ch;
    ch = test_msg[a_char];
    a_dot = ch;
    ch = test_msg[d_char];
    d_dot = ch << 6;
    ch = test_msg[g_char];
    g_dot = ch << 4;
    
}


void behlogo_bits(void)
{
    unsigned char ch;
    ch =test_msg[b_char];
    b_dot = ch <<2;
    ch = test_msg[e_char];
    e_dot = ch;
    ch = test_msg[h_char];
    h_dot = ch<<6;
    
}


void cflogo_bits(void)
{
    unsigned char ch;
    ch = test_msg[c_char];
    c_dot = ch <<4;
    ch = test_msg[f_char];
    f_dot = ch <<2;
    
}


void dot_1(void)
{
	hi_pul();
        adg_dots();
        lo_pul();
        hi_pul();
	strobes_off();
        beh_dots();
        lo_pul();
        hi_pul();
        strobes_off();
        cf_dots();
        lo_pul();
        hi_pul();
	strobes_off();
	if(ac==8)
    		a_dot = test_msg[++a_char];
	else if(ac==16)
    		a_dot = test_msg[++a_char];
	else
    		a_dot = a_dot << 1;
	if(bc==6)
    		b_dot = test_msg[++b_char];
	else if(bc==14)
    		b_dot = test_msg[++b_char];	
	else
    		b_dot = b_dot << 1;
	if(cc==4)
    		c_dot = test_msg[++c_char];
	else if(cc==12)
    		c_dot = test_msg[++c_char];
	else
    		c_dot = c_dot << 1;
	if(dc==2)
    		d_dot = test_msg[++d_char];
	else if(dc==10)
    		d_dot = test_msg[++d_char];
	else
    		d_dot = d_dot << 1;
	if(ec==8)
    		e_dot = test_msg[++e_char];
	else if(ec==16)
    		e_dot = test_msg[++e_char];
	else
    		e_dot = e_dot << 1;
	if(fc==6)
    		f_dot = test_msg[++f_char];
	else if(fc==14)
    		f_dot = test_msg[++f_char];
	else
    		f_dot = f_dot << 1;
	if(gc==4)
    		g_dot = test_msg[++g_char];
	else if(gc==12)
    		g_dot = test_msg[++g_char];
	else
    		g_dot = g_dot << 1;
	if(hc==2)
    		h_dot = test_msg[++h_char];
	else if(hc==10)
    		h_dot = test_msg[++h_char];
	else
    		h_dot = h_dot << 1;
	ac++;
	bc++;
	cc++;
	dc++;
	ec++;
	fc++;
	gc++;
	hc++;
}


void  adg_dots(void)
{
	unsigned char  temp;
   	temp = 0x00;                        //ff;

     	if((a_dot & 0x80))
        	temp = (temp | 0x01);   		//& 0xfe);
     	if((d_dot & 0x80))
        	temp = (temp | 0x08);      	//& 0xf7);
   	if((g_dot & 0x80))
        	temp = (temp | 0x40);      	//& 0xbf);
        strobes_dots(temp);
}


void beh_dots(void)
{
	unsigned char temp;		
    	temp =0x00;     			//ff;
    	if((b_dot & 0x80))
       		temp = (temp | 0x02);   		// & 0xfd);
 	if((e_dot & 0x80))
       		temp = (temp | 0x10);  		//& 0xef);
	if((h_dot & 0x80))
       		temp = (temp | 0x80);   		//&0x7F);
	strobes_dots(temp);
}
void cf_dots(void)
{
	unsigned char temp;  
    	temp =0x00;                 //ff;
	if((c_dot & 0x80))
       		temp = (temp | 0x04);  	// & 0xfb);
        if((f_dot & 0x80))
         	temp = (temp | 0x20);  //& 0xdf);
	strobes_dots(temp);
}

void strobes_dots(int temp1)
{
	if(temp1 & 0x01)
		Set(STB0,1);         
	else
        	Set(STB0,0);
	
	if(temp1 & 0x02)
        	Set(STB1,1);
	else
		Set(STB1,0);
	
	if(temp1 & 0x04)
        	Set(STB2,1);
	else
		Set(STB2,0);
	
	if(temp1 & 0x08)
        	Set(STB3,1);
	else
		Set(STB3,0);

	if(temp1 & 0x10)
        	Set(STB4,1);
	else
		Set(STB4,0);

	if(temp1 & 0x20) 
        	Set(STB5,1);
	else
		Set(STB5,0);

	if(temp1 & 0x40)
        	Set(STB6,1);
	else
		Set(STB6,0);
	
	if(temp1 & 0x80)
     		Set(STB7,1);
	else
		Set(STB7,0);

}

void chk(void)
{
      unsigned char chk_cnt;

      while(!gpio_get_value(RSTPIN));

      for(chk_cnt=0;chk_cnt<6;chk_cnt++)
      {
        hi_pul();
        lo_pul();
      }
 
      while(gpio_get_value(RSTPIN));
      while(!(gpio_get_value(TIMCNT)));
      while((gpio_get_value(TIMCNT)));
      while(!(gpio_get_value(TIMCNT)));
}

void hi_pul(void)
{
 int hi_cnt;

  while(!(gpio_get_value(TIMCNT)));
  for(hi_cnt=0;hi_cnt<50;hi_cnt++);
  //udelay(20);
  while(!gpio_get_value(TIMCNT));
}



void lo_pul(void)
{
 int lo_cnt;       
	while(gpio_get_value(TIMCNT));
	for(lo_cnt=0;lo_cnt<50;lo_cnt++);
	//udelay(20);
        while(gpio_get_value(TIMCNT));
}


/*
void sm_prt(unsigned char prn_msgch)
{

if(prn_msgch == 0x0a)
        {
                        while(smprn_i<24)
                        {
                		test_msg[smprn_i]=0x20;
                                smprn_i++;
                        }


            //    st_prt_dh();
           // }
           // else
            {
                st_prt();
            }
                        smprn_i=0;
        }
    else
        {
        test_msg[smprn_i] = prn_msgch;
           smprn_i++;

        if(smprn_i>=24)
        {
         //   if(prt_dh_flag == 1)
         //   {
         //       st_prt_dh();
         //   }
         //   else
            {
                st_prt();
            }
            smprn_i=0;
        }
        }

}
*/

/*
void prt_msg(char * prt_msg_data,unsigned char max_char)
{

 unsigned char count,prt_msg_ch;

    for(count=0;count<max_char;count++)
    {
        prt_msg_ch= prt_msg_data[count];
        sm_prt(prt_msg_ch);
    }


}


void prt_msgb(char * prt_msg_data)
{
    unsigned char count,prt_msg_ch;

    count=0;
    while(prt_msg_data[count] !='\0')
    {
            prt_msg_ch= prt_msg_data[count];
            sm_prt(prt_msg_ch);
            count++;
            if(count==24) break;
    }

    for(;count<24;count++)
    {
      prt_msg_ch=0x20; //space
      sm_prt(prt_msg_ch);
    }
}
*/

/*
void PrintPower(unsigned char Status)
{
        if(Status == ON)
        {
                at91_set_gpio_value(PRN_PWR,0);
	        at91_set_gpio_value(MOT_PWR,0);
        }
        else if(Status == OFF)
        {
                at91_set_gpio_value(PRN_PWR,1);
	        at91_set_gpio_value(MOT_PWR,1);
	}
}
*/

//This is printer power for am335x kernel for terrain...............//amar
//void PrinterPower(unsigned char State)
void PrintPower(unsigned char State)
{
	switch(State){
	case ON: 		
		Set(PRN_PWR, 1);
		Set(MOT_PWR, 1);
		break;	
	case OFF:
		 Set(PRN_PWR, 0);
		 Set(MOT_PWR, 0);
		break;
	}
}
void strobes_off(void)
{
        Set(STB0,0);         //STROBES OFF
	Set(STB1,0);
	Set(STB2,0);
	Set(STB3,0);
	Set(STB4,0);
	Set(STB5,0);
	Set(STB6,0);
	Set(STB7,0);
}


void strobes_on(void)
{

        Set(STB0,1);         //STROBES ON
	Set(STB1,1);
	Set(STB2,1);
	Set(STB3,1);
	Set(STB4,1);
	Set(STB5,1);
	Set(STB6,1);
	Set(STB7,1);
}


static int user_prn_read(struct file *file, char *buf, unsigned char count, loff_t *ppos)
{
	wait_event_interruptible(prin_queue,weventp!=0);
        copy_to_user(buf,"001",3);
        return 0;
}

static int user_prn_write(struct file * file,unsigned char *buf, unsigned int count) 
{	
	unsigned int Cnt,BCnt,Temp;
	unsigned int BHeight;
	int ret;

	memset(temp_buff,NULL_TERM,sizeof(temp_buff));
	weventp = 0;
	ret = copy_from_user(temp_buff,buf,count);
	sscanf(&buf[count],"%04d",&BHeight);
	BCnt = 0;
	printk("image write\nHight=%04d\n\r",BHeight);
	PrintPower(ON);
	spin_lock_irq(Prnblock);
	powerflag=1;
	for(Cnt = 0;Cnt < BHeight;Cnt++)
	{
		udelay(1000);		
		chk();
		for(Temp=0;Temp<18;Temp++,BCnt++)		
			test_msg[Temp] = temp_buff[BCnt];
		dot_ln();
	}
	spin_unlock_irq(Prnblock);
	PrintPower(OFF);
	powerflag=0;
	weventp=1;
	wake_up_interruptible(&prin_queue);
	//printk("\nfunctionname=%s LINE=%d\n",__func__,__LINE__); 
	return 0;
}

/*
static int user_prn_ioctl(struct inode *k,struct file *file ,unsigned char s,unsigned char *userbuff)
{
       //memset(panel_buff,NULL_TERM,sizeof(panel_buff));
       //ret = copy_from_user(panel_buff,userbuff,strlen(userbuff));   
       if(!powerflag)	
		PrintPower(s);
	return(0);
}
*/        
static int user_prn_ioctl(struct inode *k,struct file *file ,char *buf,unsigned char count,unsigned char *userbuff)
{
        int i,ret;
        char temp_buff[50];
	
	memset(test_msg,NULL_TERM,sizeof(test_msg));

        ret = copy_from_user(temp_buff,buf,count);

        printk("BUFF=%s\n",temp_buff);

        for(i=0;i<24;i++)
        test_msg[i]='\0';

        for(i=0;i<24;i++)
                test_msg[i]=temp_buff[i];

        st_prt();

        printk("IOCTL SUCCESSD\n");
       return(0);
}

//table id added by arun analogics
static const struct of_device_id printer_match_table[] = {
                {.compatible = "Analogics,2I", },
                { },
};



static const struct file_operations user_prn_file_ops = {
        .owner           = THIS_MODULE,
        .read            = user_prn_read,
        .write           = user_prn_write,
        .unlocked_ioctl  = user_prn_ioctl,
};

static struct miscdevice impctprn = {
//	MISC_DYNAMIC_MINOR,
	47,	
        "Imp2PrnDev",
        &user_prn_file_ops,
};

static int __init impctprn_init(void)
{
        int ret;

        ret = misc_register(&impctprn);
	if(ret<0)
	    printk("unable to regestered printer driver\n");
	else
            printk("\nMisc Driver for Printer Is Registered\n");
        ret=9; 
        ret=Gpio_set_value(PRN_PFO,"PFP",1,0);    	//at91_set_GPIO_periph(PFD,1);
	//printk("PFEED Ret=%d\n",ret);
        ret=Gpio_set_value(PRN_PWR,"PPOWER",1,0);	//at91_set_GPIO_periph(PRN_PWR,1);
	//printk("PRN_PWR Ret=%d\n",ret);
        ret=Gpio_set_value(STB0,"STB0",1,0);		//at91_set_GPIO_periph(STB0,1);
	//printk("STB0 Ret=%d\n",ret);
	ret=Gpio_set_value(STB1,"STB1",1,0);  		//at91_set_GPIO_periph(STB1,1);
	//printk("STB1 Ret=%d\n",ret);
        ret=Gpio_set_value(STB2,"STB2",1,0);   		//at91_set_GPIO_periph(STB2,1);
        //printk("STB2 Ret=%d\n",ret);
        ret=Gpio_set_value(STB3,"STB3",1,0);		//at91_set_GPIO_periph(STB3,1);
        //printk("STB3 Ret=%d\n",ret);
        ret=Gpio_set_value(STB4,"STB4",1,0);		//at91_set_GPIO_periph(STB4,1);
	//printk("STB4 Ret=%d\n",ret);
        ret=Gpio_set_value(STB5,"STB5",1,0);		//at91_set_GPIO_periph(STB5,1);
	//printk("STB5 Ret=%d\n",ret);
        ret=Gpio_set_value(STB6,"STB6",1,0);		//at91_set_GPIO_periph(STB6,1);
	//printk("STB6 Ret=%d\n",ret);
        ret=Gpio_set_value(STB7,"STB7",1,0);		//at91_set_GPIO_periph(STB7,1);
	//printk("STB7 Ret=%d\n",ret);
        ret=Gpio_set_value(MOT_PWR,"MPOWER",1,0);	//at91_set_GPIO_periph(MOT_PWR,1);
	//printk("MOT_PWR Ret=%d\n",ret);
	Set(PRN_PFO,0);	
        /*****************************/
        ret=Gpio_get_value(TIMCNT,"TIMECOUNT");
	//printk("TIMECNT Ret=%d\n",ret);
        ret=Gpio_get_value(RSTPIN,"RESETPIN");
	//printk("RSTPIN  Ret=%d\n",ret);
       /*********************************/
       
 	/*while(1)
	{
		Set(PRN_PFO,0);Set(PRN_PWR,0);Set(STB0,0);Set(STB1,0);Set(STB2,0);Set(STB3,0);
		Set(STB4,0);Set(STB5,0);Set(STB6,0);Set(STB7,0);Set(MOT_PWR,0);
		udelay(1000);
		Set(PRN_PFO,1);Set(PRN_PWR,1);Set(STB0,1);Set(STB1,1);Set(STB2,1);Set(STB3,1);
		Set(STB4,1);Set(STB5,1);Set(STB6,1);Set(STB7,1);Set(MOT_PWR,1);
		udelay(1000);

	}*/
	
	/**********************************/
	init_waitqueue_head(&prin_queue); 
	spin_lock_init(Prnblock);	
	printk("PRINTER GPIO's Initialization Done\n");
        return 0;
}

module_init(impctprn_init);

static void __exit impctprn_exit(void)
{
        misc_deregister(&impctprn);
}

module_exit(impctprn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KISHORE,ANALOGICS");
MODULE_DESCRIPTION("2'' IMPACT PRINTER DRIVER");
                                             
