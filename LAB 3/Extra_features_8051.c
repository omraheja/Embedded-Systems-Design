/*Author Name : Om Raheja
  Date        : 20th October 2018
  Course      : ECEN 5613 Embedded System Design
  Project     : UART DYNAMIC MEMORY MANAGEMENT
*/

/* ************************************************************************************************************************* */
/*                                                 START OF PROGRAM                                                          */
/* ************************************************************************************************************************* */

/* ************************************************************************************************************************* */
/*                                             C STANDARD LIBRARY HEADERS                                                    */
/* ************************************************************************************************************************* */

#include <mcs51/8051.h>
#include <mcs51/at89c51ed2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* ************************************************************************************************************************* */
/*                                                 FUNCTION PROTOTYPES                                                       */
/* ************************************************************************************************************************* */
void putchar(char c);
char getchar();
int take_input();
void power_down(void);
void pwm(void);
void idle_mode(void);


/* ************************************************************************************************************************* */
/*                                                   PUTCHAR FUNCTION                                                        */
/* ************************************************************************************************************************* */

/*Function for Putchar*/
void putchar(char c)
{
    while(!TI);
    SBUF=c;
    TI=0;
}

/* ************************************************************************************************************************* */
/*                                                   GETCHAR FUNCTION                                                        */
/* ************************************************************************************************************************* */

/*Function for getchar*/
char getchar ()
{
    while (!RI);                		 // compare asm code generated for these three lines
	RI = 0;			            	 // clear RI flag
	return SBUF;  	            		 // return character from SBUF
}


/* ************************************************************************************************************************* */
/*                                                   TAKE INPUT FUNCTION                                                     */
/* ************************************************************************************************************************* */

/*Function to take input from user*/
int take_input()
{
    int flag=0;
    unsigned int a=0;
    unsigned int num=0;

    do
    {
        printf_tiny("\n\rEnter the serial number\n\r");
        a=getchar();
        putchar(a);

        /*Conversion from ASCII to INTEGER*/
        num=(a-48);

        if(num>=1 && num <=6)
        {
            flag=0;
        }
        else
        {
            printf_tiny("\n\rINVALID INPUT\n\rENTER AGAIN\n\r");
            flag=1;
        }
    }while(flag==1);
    return num;
}


/* ************************************************************************************************************************* */
/*                                                   POWER DOWN FUNCTION                                                     */
/* ************************************************************************************************************************* */

/*Power down mode*/
void power_down(void)
{
    PCON |=0x02;
}

/* ************************************************************************************************************************* */
/*                                         PULSE WIDTH MODULATION (PWN) FUNCTION                                             */
/* ************************************************************************************************************************* */

void pwm(void)
{
        CMOD |= 0x03;                            //PCA count pulse select;Internal clock=F(clkperiph)/2
        CCAPM1 |=0x42;                           //PCA module 1 Compare/Capture control register;enable comparator;Pulse width modulation mode
        CCAP1H = 0x8d;          		 //255*55/100~141   8d(hex)=141(dec)
}

/* ************************************************************************************************************************* */
/*                                                 IDLE MODE FUNCTION                                                        */
/* ************************************************************************************************************************* */

/*Idle mode function*/
void idle_mode(void)
{
    PCON |=0x01;
}

/* ************************************************************************************************************************* */
/*                                                    MAIN FUNCTION                                                          */
/* ************************************************************************************************************************* */

void main(void)
{
    unsigned int number;
    number=0;

    pwm();
    printf_tiny("\n\r------------------------MENU----------------------------------------------\r");
    printf_tiny("\n\r>>1) Run PWM\n\r");
    printf_tiny("\n\r>>2) Stop PWM\n\r");
    printf_tiny("\n\r>>3) Set Peripheral frequency at minimum frequency by CKRL register\n\r");
    printf_tiny("\n\r>>4) Set Peripheral frequency at maximum frequency by CKRL register\n\r");
    printf_tiny("\n\r>>5) Enter in Idle mode\n\r");
    printf_tiny("\n\r>>6) Enter power down mode\n\r");
    while(1)
    {
        printf_tiny("\n\rEnter the serial number to go in the desired mode\n\r");
        number=take_input();

        if(number==1)
        {
            printf_tiny("\n\r\PWM ON\n\r");
            CR=1;                	      	 //PWM on
        }
        else if(number==2)
        {
            printf_tiny("\n\r\PWM OFF\n\r");
            CR=0;                		 //PWM off
        }
        else if(number ==3)
        {
            CKRL =0x00;                   	 //Minimum frequency
        }
        else if(number ==4)
        {
            CKRL =0xFF;                          //Maximum frequency
        }
        else if(number ==5)
        {
            idle_mode();                         //call idle mode
        }
        else if(number ==6)
        {
            power_down();
        }
    }
}

/* ************************************************************************************************************************* */
/*                                                   END OF PROGRAM                                                          */
/* ************************************************************************************************************************* */
