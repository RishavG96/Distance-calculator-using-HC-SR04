/**************************************************************************
 
 *-------------------------------------------------------------------------
 * Controller	: LPC1768
 * Project		: ALS-SDA-AMCTXM3-01
 * Port lines used: Data1 to Data4 - P0.23 to P0.26
 * En - P0.28. RS - P0.27, RW - Ground
 * Connection : CND to CNAB
 ***************************************************************************/

 #include <lpc17xx.h>

 #include "AN_LCD.h"

 unsigned long int temp1=0, temp2=0 ;

 //lcd initialization
 void lcd_init()
 {
	/* Ports initialized as GPIO */
   // LPC_PINCON->PINSEL0 &= 0x00000000;  //P0.23 to P0.28

	/* Setting the directions as output */
    LPC_GPIO0->FIODIR |= DT_CTRL;	
	LPC_GPIO0->FIODIR |= RS_CTRL;	
	LPC_GPIO0->FIODIR |= EN_CTRL;	
        
    clear_ports();
	delay_lcd(10000);

	temp2 = (0x30<<19);
	wr_cn();	   
	delay_lcd(25000); 
		
	temp2 = (0x30<<19);
	wr_cn();
	delay_lcd(25000);	 
		
	temp2 = (0x30<<19);
	wr_cn();
	delay_lcd(25000);

	temp2 = (0x20<<19);
	wr_cn();
	delay_lcd(25000);

	temp1 = 0x28;
	lcd_com();
	delay_lcd(25000);
		
	temp1 = 0x0C;		
	lcd_com();
	delay_lcd(20800);
	
	temp1 = 0x06;
	lcd_com();
	delay_lcd(28000);
	
	temp1 = 0x01;
	lcd_com();
 	delay_lcd(5000);
	
	temp1 = 0x80;
	lcd_com();
	delay_lcd(800);
    return;
 }

 void lcd_com(void)
 {
	temp2 = temp1 & 0xf0;//move data (26-8+1) times : 26 - HN place, 4 - Bits
	temp2 = temp2 << 19;//data lines from 23 to 26
	wr_cn();
	temp2 = temp1 & 0x0f; //26-4+1
	temp2 = temp2 << 23; 
	wr_cn();
	delay_lcd(1000);
    return;
 }

 // command nibble o/p routine
 void wr_cn(void)                        //write command reg
 { 	 
	clear_ports();
	LPC_GPIO0->FIOPIN = temp2;		// Assign the value to the data lines    
    LPC_GPIO0->FIOCLR = RS_CTRL;		// clear bit RS
	LPC_GPIO0->FIOSET = EN_CTRL;   	// EN=1
	delay_lcd(25);
	LPC_GPIO0->FIOCLR  = EN_CTRL;		 		// EN =0
    return;
    
 }

 // data o/p routine which also outputs high nibble first
 // and lower nibble next
 void lcd_data(void)
 {             
    temp2 = temp1 & 0xf0;
    temp2 = temp2 << 19;
    wr_dn();
    temp2= temp1 & 0x0f;	
    temp2= temp2 << 23;
    wr_dn();
    delay_lcd(1000);	
    return;
 } 

 // data nibble o/p routine
 void wr_dn(void)
 {  	  
	clear_ports();

	LPC_GPIO0->FIOPIN = temp2;		// Assign the value to the data lines    
	LPC_GPIO0->FIOSET = RS_CTRL;	// set bit  RS
	LPC_GPIO0->FIOSET = EN_CTRL;   	// EN=1
	delay_lcd(25);
	LPC_GPIO0->FIOCLR = EN_CTRL;	// EN =0
    return;
 }

 void delay_lcd(unsigned int r1)
 {
  	unsigned int r;
  	for(r=0;r<r1;r++);
    return;
 }

 void clr_disp(void)
 {
	temp1 = 0x01;
	lcd_com();
 	delay_lcd(10000);
    return;
 }
 void clear_ports(void)
 {
    /* Clearing the lines at power on */
	LPC_GPIO0->FIOCLR = DT_CTRL; //Clearing data lines
	LPC_GPIO0->FIOCLR = RS_CTRL; //Clearing RS line
	LPC_GPIO0->FIOCLR = EN_CTRL; //Clearing Enable line
        
    return;
 }

 void lcd_puts(unsigned char *buf1)
 {
    unsigned int i=0;

    while(buf1[i]!='\0')
    {
        temp1 = buf1[i];
     	lcd_data();
		i++;
        if(i==16)
		{
           	temp1 = 0xc0;
			lcd_com();
		}
         
       }
    return;
 }
