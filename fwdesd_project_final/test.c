#include<LPC17xx.h>
#include<stdio.h>
#include"AN_LCD.h"

//TIMER0 IS FOR TRIGGER INPUT
//TIMER1 IS FOR TRIGGER OUTPUT

float x;
 unsigned int i,j;
unsigned char dist[20]; // array for putting distance value in lcd
unsigned char Msg3[] = {"Dist:"};
unsigned char Msg4[] = {"WELCOME"};
unsigned char Msg5[] = {"RANGE FINDER!"};
void TIMER0_IRQHandler(void) // TO SEND PULSE FOR 10us IN TRIGGER 
{
	LPC_TIM0->IR=0x01;
	LPC_GPIO2->FIOCLR=0x00000400;// CLEARING TRIGGER
	LPC_TIM0->TCR=0x00;
	LPC_TIM3->TCR=0x02;
	LPC_TIM3->TCR=0x01;
}
void TIMER3_IRQHandler(void)// TO START 60ms TIMER SO THAT IF NO OBJECT IS DETECTED IT STARTS THE PROCESS ALL OVER AGAIN
{
	LPC_TIM3->IR=0x01;
	LPC_TIM3->TCR=0x00;
	LPC_TIM0->TCR=0x02;
	LPC_TIM0->TCR=0x01;
	LPC_GPIO2->FIOSET=0x00000400;// SETTING TRIGGER
}
void EINT1_IRQHandler(void)// TO GET THE LENGTH OF THE PULSE IN ECHO FOR DISTANCE CALCULATION
{
	
	LPC_SC->EXTINT=0x02;//clear EINT1 interr
	LPC_TIM3->TCR=0x00; // SO THAT NOW A POSITIVE EGDE IS ENCOUNTERED THUS 60NS TIMER CAN BE DISABLED
	
	i=LPC_SC->EXTPOLAR; // GETTING WHICH EGDE TRIGGER IS ENCOUNTERED +ve OR -ve
	LPC_TIM1->TCR=0x00;
 x=0.0;
	i&=0x02;//EINT1 status extract
	i>>=1;
	
	if(i==0x01)// POSITIVE EGDE ENCOUNTERED
	{
		LPC_SC->EXTPOLAR=0x00;
		LPC_TIM1->TCR=0x02;
	LPC_TIM1->TCR=0x01; //STARTNG TIMER FOR DISTANCE CALCULATION
	
	}
	else// NEGETIVE EDGE ENCOUNTERED
	{
		
		LPC_SC->EXTPOLAR=0x02;
	j=LPC_TIM1->TC; // CAPTURING VALUE OF TC
	x=j/58.0; // us/58 OR HighLevelTime*340/2
			temp1 = 0x85;
		lcd_com();
		delay_lcd(800);

	sprintf(dist,"%3.2fcm   ",x); //CONVERTING FLOAT INTO STRING
	
		lcd_puts(&dist[0]); // SHOW DISTANCE IN LCD
		delay_lcd(80000);
	}
	LPC_TIM0->TCR=0x02;
	LPC_TIM0->TCR=0x01;
	LPC_GPIO2->FIOSET=0x00000400;// SETTING TRIGGER
	
}
int main(void)
{
	SystemInit();
	SystemCoreClockUpdate();
lcd_init();
	temp1 = 0x80;
	lcd_com();
	delay_lcd(800);
	lcd_puts(&Msg4[0]);// to display welcome
	delay_lcd(2000000);
	clr_disp();// clear LCD
	
	temp1 = 0x80;
	lcd_com();
	delay_lcd(800);
	lcd_puts(&Msg5[0]);// to display Range Finder!
	delay_lcd(2000000);
	clr_disp();// clear LCD
	
LPC_PINCON->PINSEL4|=0x00000000; //P2.10 AS TRIGGER
LPC_PINCON->PINSEL4=(1<<22); // P2.11 AS FUNCTION-1 FOR EXT INTERRUPT
LPC_GPIO2->FIODIR=0x400;// SETTING P2.10 AS OUTPUT

	temp1 = 0x80;
	lcd_com();
	delay_lcd(800);
	lcd_puts(&Msg3[0]);// to display Dist:
// configure external interrupt for positive edge interrupt
	LPC_GPIO2->FIOPIN=0x00000000;
	LPC_SC->EXTMODE=0x02; // EDGE FOR EINT1
	LPC_SC->EXTPOLAR=0x02; // RISING EDGE FOR EINT1
	LPC_SC->EXTINT=0x0F;
 //configure timer0 for 10us trigger pulse
	LPC_TIM0->TCR=0x02;
	LPC_TIM0->CTCR=0x00;
	LPC_TIM0->MR0=29;
	LPC_TIM0->PR=0;
	LPC_TIM0->MCR=0x03;
// configuring timer1 for capturing TC value in us	
	LPC_TIM1->TCR=0x02;
	LPC_TIM1->PR=2; // FOR TC IN SECONDS
	
// configuring timer3 for 60ms timer after pulse is generated from trigger so that if within 60ms no object is found restart the whole process	
	LPC_TIM3->TCR=0x02;
	LPC_TIM3->MR0=179999;
	LPC_TIM3->EMR=0x30;
	LPC_TIM3->PR=0;
	LPC_TIM3->MCR=0x03;
	// TIMER0 FOR 10us TRIGGER PULSE
	
	
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER3_IRQn);
	
	LPC_SC->EXTMODE=0X02; // EDGE FOR EINT1
	LPC_SC->EXTPOLAR=0X02; // RISING EDGE FOR EINT1
	LPC_GPIO2->FIOCLR = 0X00000400;
	LPC_TIM0->EMR=0x30;
	
	LPC_TIM3->TCR=0x02;
	LPC_TIM3->TCR=0x01; // staring 10us timer
	
	
while(1);
}
