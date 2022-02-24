/*
 * MC1.c
 *
 *  Created on: Jul 30, 2021
 *      Author: Mostafa Mohsen
 */
#include "lcd.h"
#include "keypad.h"
#include"uart.h"
#include"timer.h"
#define NO_OVERFLOWS_SECOND 16
#define OKPASS 0x12
#define OKREPASS 0x13
#define MATCHED 0x14
#define UNMATCHED 0x15
#define OPENED 0x16
#define CLOSED 0x17
#define THIEF 0x18
#define PLUS  0x22
#define MINUS 0x23
char password[10];
char repassword[10];
uint8 tick=0;
uint8 seconds=0;
uint8 seconds_needed=0;
uint8 m=0;

void ISR_COUNT(void)
{
	tick++;
	if(tick==NO_OVERFLOWS_SECOND)
	{
		seconds++;
		tick=0;
		if(seconds==seconds_needed)
		{
			seconds=0;
			timer0_DeInit();
				m=1;
		}
	}
}
void empty_password(char *str)
{
	uint8 j=0;
	while(str[j]!='\0')
		{
		str[j]='\0';
		j++;
		}
}
void password_Init()
{
	repassword[0]='6';
	repassword[1]='8';
	password[0]='4';
	password[1]='8';
}
void Entering_password(uint8 a)
{
	password_Init();
	char buff;
    uint8 i=2;
	uint8 key;
	LCD_clearScreen();
	if(a==0)
	   LCD_displayStringRowColumn(0,0,"Enter password:");
	else if(a==1)
		LCD_displayStringRowColumn(0,0,"Re-enter pass:");
			LCD_goToRowColumn(1,0);
			while(i<10)
			{
				key=KeyPad_getPressedKey();
				if(key== '=')
				{
					if(a==0)
					password[i]='#';
					else
					repassword[i]='#';
					break;
				}
				if((key <= 9) && (key >= 0))
				{
					LCD_displayCharacter('*');
					   buff=key+'0';
					   if(a==0)
					   password[i]=buff;
					   else
					   repassword[i]=buff;
				}
				_delay_ms(500);
				i++;
			}
}
void Checkingpassword(uint8 type)
{
	    uint8 detector=0;//used to detect if i send or recieve pass word
		uint8 outing=0;
		uint8 recievematching;
		while(1)
		{
		empty_password(password);
		empty_password(repassword);
		Entering_password(detector);
		LCD_clearScreen();
		// initialize UART // wait until MC2 is ready
		UART_sendString(password);
		_delay_ms(500);
		//while(UART_recieveByte()!=OKPASS){}
		recievematching=UART_recieveByte();
		if(recievematching==OKPASS)
		{
		detector=1;
		LCD_clearScreen();
		LCD_goToRowColumn(1,0);
		Entering_password(detector);
		}
		LCD_clearScreen();
		UART_sendString(repassword);
		_delay_ms(500);
		//while(UART_recieveByte()!=OKREPASS){}
		recievematching=UART_recieveByte();
		if(recievematching==OKREPASS)
		{
		LCD_clearScreen();
	   recievematching=UART_recieveByte();
	   if(recievematching==MATCHED)
	   {
		   switch (type)
		   {
		   case 0:
		   LCD_displayStringRowColumn(0,0,"MATCHED");
		   		outing=1;
		   		break;
		   case 1:
			   LCD_displayStringRowColumn(0,0,"Pass Changed");
			   break;
		   }
		   _delay_ms(1500);
		   	LCD_clearScreen();
	   }
		else if(recievematching==UNMATCHED)
		   {
			   switch(type)
			   {
			   case 0:
			      LCD_displayStringRowColumn(0,0,"UNMATCHED");
			      break;
			   case 1:
			  	  LCD_displayStringRowColumn(0,0,"Not Equal");
			  	  break;
			   }
			   _delay_ms(1500);
			    LCD_clearScreen();
		   }
		}
		if(outing||type)
			break;
		detector=0;
		}
}
void checkingdoor(void)
{
	uint8 errors=0;
		uint8 recieveopening;
	while(errors<=3)
				 {
		if(errors==3)
			break;
				  Entering_password(0);
				  LCD_clearScreen();
				  UART_sendString(password);
				  recieveopening=UART_recieveByte();
    			  recieveopening=UART_recieveByte();
				  if(recieveopening==OPENED)
				  {
					  LCD_clearScreen();
					  SREG |= (1<<7);
					  seconds_needed=10;
					  Set_callBack(ISR_COUNT);
					  Timer0_ConfigType config={CTC,F_CPU_256,250};
					  LCD_displayStringRowColumn(0,0,"Opened");
					  Timer0_init(&config);
					  while(m==0);
					  LCD_clearScreen();
					  m=0;
					  LCD_displayStringRowColumn(0,0,"Closed");
					  Timer0_init(&config);
					  while(m==0);
					 errors=0;
					 break;
				  }
				  else if(recieveopening==CLOSED)
				  {
				  errors++;
					  if(errors==3)
					  {
						  SREG|=(1<<7);
						  seconds_needed=20;
						  Set_callBack(ISR_COUNT);
						  Timer0_ConfigType config={CTC,NO_CLOCK,250};
						  LCD_displayStringRowColumn(0,0,"There is Thief");
						  Timer0_init(&config);
						  UART_sendByte(THIEF);
						  while(m==0);
						  LCD_clearScreen();
					  }
					  else
					  {

					  LCD_displayStringRowColumn(0,0,"Try again");
					  _delay_ms(1000);
					  }
				  }
				  }
}
int main(void)
{
	uint8 typer=0;
	uint8 key;
	LCD_init();
    UART_init();
	Checkingpassword(typer);
	LCD_clearScreen();
	while(1)
	{
		  LCD_displayStringRowColumn(0,0,"+ Opendoor");
		  LCD_displayStringRowColumn(1,0,"- Change pass");
		  key=KeyPad_getPressedKey();
		  if(key=='+')
		  {
			  UART_sendByte(PLUS);
			  checkingdoor();
		  }
		  else if(key=='-')
		  {
              typer=1;
			  UART_sendByte(MINUS);
			  Checkingpassword(typer);
		  }
		  else
		  {
			  LCD_clearScreen();
			  LCD_displayStringRowColumn(0,0,"Invalid");
			  _delay_ms(500);
		  }
	}
}

