#include "uart.h"
#include "lcd.h"
#include "external_eeprom.h"
#include"timer.h"
#define NO_OVERFLOWS_SECOND 16
#define OKPASS 0x12
#define OKREPASS 0x13
#define MATCHED 0x14
#define UNMATCHED 0x15
#define OPENED 0x16
#define CLOSED 0x17
#define THIEF 0x18
#define CHECKING_DOOR 0x19
#define CHECKING_PASS 0x20
#define PLUS  0x22
#define MINUS 0x23
char eeprompass[10];
char pass[10];
char repass[10];
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
uint8 strcmp(const char *p,const char *e)
{
	uint8 i=0;
	uint8 matched=1;
	while(pass[i]!='\0'||repass[i]!='\0')
			{
				if(pass[i]!=repass[i])
				{
					matched=0;
				    return 0;
				}
				i++;
			}
	return 1;
}
void strcpy(const char *old, char *new)
{
	uint8 i=0;
	while(old[i]!='\0')
	{
		new[i]=old[i];
		i++;
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
void Reciving_password(uint8 *Str)
{
	uint8 i=0;
	if(Str[0]=='4'&&Str[1]=='8')
			{
		empty_password(pass);
		     while(Str[i+2] != '\0')
		     {
		    	 pass[i]=Str[i+2];
		    	 i++;
		     }
		     UART_sendByte(OKPASS);
		     	_delay_ms(2000);
			}
	else if(Str[0]=='6'&&Str[1]=='8')
	{
		empty_password(repass);
		 while(Str[i+2] != '\0')
		     {
		    	 repass[i]=Str[i+2];
		    	 i++;
		     }
		     UART_sendByte(OKREPASS);
		     	LCD_displayString(repass);
		     	_delay_ms(2000);
	}
}
void MC2_checking(void)
{
	uint8 i=0;
	uint8 Str[12];
		uint8 matched=1;
		while(1)
		{
		   matched=1;
		 empty_password(pass);
		 empty_password(repass);
		   UART_receiveString(Str);
		   Reciving_password(Str);
		LCD_clearScreen();
		UART_receiveString(Str);
		Reciving_password(Str);

		LCD_clearScreen();
		while(pass[i]!='\0'||repass[i]!='\0')
		{
			if(pass[i]!=repass[i])
			{
				matched=0;
			    break;
			}
			i++;
		}
		if(matched)
		{
			UART_sendByte(MATCHED);
			strcpy(pass,eeprompass);
			break;
		}
		else
			UART_sendByte(UNMATCHED);
		}
}
void Thiefmode(void)
{
 DDRC|=(1<<PC6);
 PORTC &=0xBF;
 SREG |= (1<<7);
 seconds_needed=20;
 Set_callBack(ISR_COUNT);
 Timer0_ConfigType config={CTC,F_CPU_256,250};
 PORTC|=(1<<PC6);
 Timer0_init(&config);
 while(m==0);
 m=0;
 PORTC&=~(1<<PC6);
}
void Motor_Stepper(void)
{
		DDRC |= 0x18;  // Configure first 4 pins in PORTC as output pins
		PORTC &= 0xE7;// Clear first four bits in PORTC
		SREG |= (1<<7);
		seconds_needed=10;
	    Set_callBack(ISR_COUNT);
	    Timer0_ConfigType config={CTC,F_CPU_256,250};
	    PORTC&=~(1<<PC3);
	    PORTC|=(1<<PC4);
		Timer0_init(&config);
        while(m==0);
       m=0;
       PORTC|=(1<<PC3);
       PORTC&=~(1<<PC4);
       Timer0_init(&config);
        while(m==0);
        m=0;
        PORTC&=~(1<<PC3);
        PORTC&=~(1<<PC4);
}
void checkingdoor_uart2(void)
{
	uint8 reciver;
	uint8 counter=0;
	uint8 Str[12];
	while(counter<3)
	{
	UART_receiveString(Str);
			Reciving_password(Str);
			if(strcmp(pass,eeprompass))
			{
				UART_sendByte(OPENED);
     			Motor_Stepper();
				counter=0;
				break;
			}
			else
			{
				counter++;
				UART_sendByte(CLOSED);

			}
	}
	if(counter==3)
	{
			reciver=UART_recieveByte();
			if(reciver==THIEF)
				Thiefmode();
	}

}
int main(void)
{
	UART_init(); // initialize UART driver
	EEPROM_init();
	//initialize LCD
	MC2_checking();
	while(1)
	{
		uint8 Plus_Minus=UART_recieveByte();
        if(Plus_Minus==0x22)
		checkingdoor_uart2();
        else if(Plus_Minus==0x23)
        {
        	MC2_checking();
        	strcpy(pass,eeprompass);
        }
	}
}

