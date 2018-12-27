/*  Author: Om Raheja
    Course: ECEN 5613 Embedded System Design
    Fall 2018
    Description:Implementation of I2C protocol.
    Filename: I2C_protocol.c
    Controller: AT89C51RC2  EEPROM: Atmel AT24C16C
    Oscillator: 11.0592 MHz
*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              START OF PROGRAM                                                                 */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              STANDARD LIBRARY HEADERS                                                         */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <mcs51/8051.h>
#include <mcs51/at89c51ed2.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              DEFINE STATEMENTS                                                                */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

#define SDA P1_4                     //Define SDA as Pin 1.4
#define SCL P1_3                     //Define SCL as Pin 1.3

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              FUNCTION PROTOTYPES                                                              */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

uint16_t take_address();
uint16_t seq_read(uint16_t recv_addr);
void eereset();
void delay();
void putchar(char);
void nack();
void start();
void stop();
int ack();
char getchar();

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              DELAY FUNCTION                                                                   */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void delay()
{
    int i;
    for(i=0;i<500;i++);
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              PUTCHAR FUNCTION                                                                 */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void putchar(char c)
{
    while(!TI);
    SBUF=c;
    TI=0;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              GETCHAR FUNCTION                                                                 */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

char getchar ()
{
    while (!RI);
	RI = 0;
	return SBUF;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              NACK FUNCTION                                                                    */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void nack()
{
    SDA=1;
    delay();
    SCL=1;
    delay();
    SCL=0;
    delay();
    SDA=0;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              START CONDITION                                                                  */
/*FUNCTION DESCRIPTION: A high to low transition of the SDA bus,when SCL is high,defines the START condition.                                    */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void start()
{
    delay();
    SDA=1;       //SDA high
    delay();
    SCL=1;       //SCL high
    delay();     //Generate a delay
    SDA=0;       //SDA low
    delay();
    SCL=0;       //SCL high to low transition of clock corresponding to each bit of word
    delay();
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              STOP FUNCTION                                                                    */
/*FUNCTION DESCRIPTION: A low to high transition of the SDA bus,when SCL is high,defines the STOP condition.                                     */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void stop()
{
	SDA=0;
	delay();
	SCL=0;
	delay();
	SCL=1;
	delay();
	SDA=1;
	delay();
	SCL=0;
	delay();
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              ACK FUNCTION                                                                     */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

int ack()
{
    SDA=1;               //transmitter releases SDA bus
    delay();
    SCL=0;
    delay();
    SCL=1;
    delay();
    if(SDA==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              WRITE BYTE                                                                       */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void data_write()
{
    uint16_t address;              //to store the inputed address from the user in a single integer form
    uint16_t om_data;
    uint8_t flag;                  //flag variable to keep conditions in check
    uint8_t flag_1;                //flag_1 variable to keep conditions in check
    uint8_t i;                     //counter variable
    uint8_t j;                     //counter variable
    uint8_t byte;
    uint8_t byte_1;
    char get_data[2];
    char get_data_int[2];
    char get_address[3];           //Get address from user
    uint16_t get_address_int[3];   //store the inputed address in int array(char array-->int array)

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Variable initialization*/
    address=0;
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Enter address and check validity of address*/
    do
    {
        flag=0;
        printf_tiny("\n\rEnter the address between 0x00-0x7ff\n\r");
        for(i=0;i<3;i++)
        {
            get_address[i]=getchar();
            putchar(get_address[i]);
        }

        for(i=0;i<3;i++)
        {
            switch(i)
            {
                case 0:
                    if((get_address[i]>=48) && (get_address[i]<=55))
                    {
                        get_address_int[i]= get_address[i]-48;
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag=1;
                    }
                    break;

                case 1:
                    if(((get_address[i]>='0') && (get_address[i]<='9')) ||  ((get_address[i]>='A') && (get_address[i]<='F')) || ((get_address[i]>='a') && (get_address[i]<='f')))
                    {
                        if((get_address[i]>=48) && (get_address[i]<=57))
                        {
                            get_address_int[i]=get_address[i]-48;
                        }
                        else if((get_address[i]>=65) && (get_address[i]<=70))
                        {
                            get_address_int[i]=get_address[i]-55;
                        }
                        else if((get_address[i]>=97) && (get_address[i]<=102))
                        {
                            get_address_int[i]=get_address[i]-87;
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag=1;
                    }
                break;

                case 2:
                    if(((get_address[i]>='0') && (get_address[i]<='9')) ||  ((get_address[i]>='A') && (get_address[i]<='F')) || ((get_address[i]>='a') && (get_address[i]<='f')))
                    {
                        if((get_address[i]>=48) && (get_address[i]<=57))
                        {
                            get_address_int[i]=get_address[i]-48;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                        else if((get_address[i]>=65) && (get_address[i]<=70))
                        {
                            get_address_int[i]=get_address[i]-55;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                        else if((get_address[i]>=97) && (get_address[i]<=102))
                        {
                            get_address_int[i]=get_address[i]-87;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag=1;
                    }
                    break;

                    default:break;
            }
        }
    }while(flag==1);

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

    /*Convert address into a number which is an integer*/
    for(i=0;i<3;i++)
    {
        address = address+((get_address_int[i])*powf(16,2-i));
    }

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

    /*Perform shifting and setting byte=1010A3A2A1(w/r) <--binary form*/
    byte = ((0xA0)|(address >> 7)&(0xfe));

    /*Store the lower byte of the address in byte_1*/
    byte_1 = (uint8_t)address;

/*-------------------------------------------------------------------------------------------------------------------------------------------*/

    /*Enter data and check validity of data*/
    do
    {
        flag_1=0;
        printf_tiny("\n\rEnter the data you want to write(*Note:DATA SHOULD BE BETWEEN 0x00 and 0xFF)\n\r");

        for(i=0;i<2;i++)
        {
            get_data[i]=getchar();
            putchar(get_data[i]);
        }

        for(i=0;i<2;i++)
        {
            switch(i)
            {
                case 0:
                    if(((get_data[i]>='0') && (get_data[i]<='9')) ||  ((get_data[i]>='A') && (get_data[i]<='F')) || ((get_data[i]>='a') && (get_data[i]<='f')))
                    {
                        if((get_data[i]>=48) && (get_data[i]<=57))
                        {
                            get_data_int[i]=get_data[i]-48;
                            printf_tiny("\n\rget_data_int[%d]=%d\n\r",i,get_data_int[i]);
                        }
                        else if((get_data[i]>=65) && (get_data[i]<=70))
                        {
                            get_data_int[i]=get_data[i]-55;
                            printf_tiny("\n\rget_data_int[%d]=%d\n\r",i,get_data_int[i]);
                        }
                        else if((get_data[i]>=97) && (get_data[i]<=102))
                        {
                            get_data_int[i]=get_data[i]-87;
                            printf_tiny("\n\rget_data_int[%d]=%d\n\r",i,get_data_int[i]);
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag_1=1;
                    }
                break;

                case 1:
                    if(((get_data[i]>='0') && (get_data[i]<='9')) ||  ((get_data[i]>='A') && (get_data[i]<='F')) || ((get_data[i]>='a') && (get_data[i]<='f')))
                    {
                        if((get_data[i]>=48) && (get_data[i]<=57))
                        {
                            get_data_int[i]=get_data[i]-48;
                            printf_tiny("\n\rget_data_int[%d]=%d\n\r",i,get_data_int[i]);
                        }
                        else if((get_data[i]>=65) && (get_data[i]<=70))
                        {
                            get_data_int[i]=get_data[i]-55;
                            printf_tiny("\n\rget_data_int[%d]=%d\n\r",i,get_data_int[i]);
                        }
                        else if((get_data[i]>=97) && (get_data[i]<=102))
                        {
                            get_data_int[i]=get_data[i]-87;
                            printf_tiny("\n\rget_data_int[%d]=%d\n\r",i,get_data_int[i]);
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag_1=1;
                    }
                break;

                    default:break;
            }
        }
    }while(flag_1==1);

/*-------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Convert data into a number which is an integer*/
    for(i=0;i<2;i++)
    {
        om_data = om_data +((get_data_int[i])*powf(16,1-i));
    }
/*-------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Start function*/
    start();
/*-------------------------------------------------------------------------------------------------------------------------------------------*/

    /*To transfer the bits sequentially on the serial terminal with MSB being transferred first
      This part of the code was referenced from (https://www.8051projects.net/wiki/I2C_Implementation_on_8051#Implementing_I2C_in_C)'s
      Tutorial section.
      EXPLANATION AND WORKING OF THIS SECTION OF THE CODE:
      1) The MSB of the byte variable is being extracted and checked if it's equal to 0 or not.If its equal to 0,then SDA=0 is being sent onto
         the serial terminal else SDA=1 is being sent onto the serial terminal.
      2) Once the first MSB has been checked,a left shift has been performed to check and perform the following operation on the next bit.
      3) Step 2 is executed until 8 bits have been checked and transmitted.
      4) Once all the bits have been checked an ACK is received to inform the MASTER that all the 8 bits were received.
    */
    for (i=0;i<8;i++)
    {
		if((byte & 0x80) == 0)
        {
            SDA = 0;
            delay();
        }
		else
        {
            SDA = 1;
            delay();
        }
		SCL = 1;
		delay();
		SCL=0;
		byte<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*PRINT LOWER BYTE OF THE ADDRESS*/
	 SCL=0;
	 /*Send lower byte of address*/
	 for (j=0;j<8;j++)
    {
		if((byte_1 & 0x80) == 0)
        {
            SDA = 0;
            delay();
        }
		else
        {
            SDA = 1;
            delay();
        }
		SCL = 1;
		delay();
		SCL=0;
		byte_1<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
     /*PRINT DATA TO BE WRITTEN */
     SCL=0;
	 /*write data*/
	 for (i=0;i<8;i++)
    {
		if((om_data & 0x80) == 0)
        {
            SDA = 0;
            delay();
        }
		else
        {
            SDA = 1;
            delay();
        }
		SCL = 1;
		delay();
		SCL=0;
		om_data<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Stop function*/
	 stop();
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                    READ BYTE                                                                                  */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void data_read()
{
    uint16_t address;            //to store the inputed address from the user in a single integer form
    uint8_t flag;                //flag variable to keep conditions in check
    uint8_t i;                   //counter variable
    uint8_t j;                   //counter variable
    uint16_t bit_received;
    uint8_t byte;
    uint8_t byte_1;
    uint8_t byte_2;
    uint16_t dat;
    int z;
    char get_address[3];          //Get address from user
    uint16_t get_address_int[3];  //store the inputed address in int array(char array-->int array)

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Variable initialization*/
    address=0;
    bit_received=0x00;
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Enter address and check validity of address*/
    do
    {
        flag=0;
        printf_tiny("\n\rEnter the address between 0x00-0x7ff\n\r");
        for(i=0;i<3;i++)
        {
            get_address[i]=getchar();
            putchar(get_address[i]);
        }

        for(i=0;i<3;i++)
        {
            switch(i)
            {
                case 0:
                    if((get_address[i]>=48) && (get_address[i]<=55))
                    {
                        get_address_int[i]= get_address[i]-48;
                        printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag=1;
                    }
                    break;

                case 1:
                    if(((get_address[i]>='0') && (get_address[i]<='9')) ||  ((get_address[i]>='A') && (get_address[i]<='F')) || ((get_address[i]>='a') && (get_address[i]<='f')))
                    {
                        if((get_address[i]>=48) && (get_address[i]<=57))
                        {
                            get_address_int[i]=get_address[i]-48;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                        else if((get_address[i]>=65) && (get_address[i]<=70))
                        {
                            get_address_int[i]=get_address[i]-55;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                        else if((get_address[i]>=97) && (get_address[i]<=102))
                        {
                            get_address_int[i]=get_address[i]-87;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag=1;
                    }
                break;

                case 2:
                    if(((get_address[i]>='0') && (get_address[i]<='9')) ||  ((get_address[i]>='A') && (get_address[i]<='F')) || ((get_address[i]>='a') && (get_address[i]<='f')))
                    {
                        if((get_address[i]>=48) && (get_address[i]<=57))
                        {
                            get_address_int[i]=get_address[i]-48;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                        else if((get_address[i]>=65) && (get_address[i]<=70))
                        {
                            get_address_int[i]=get_address[i]-55;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                        else if((get_address[i]>=97) && (get_address[i]<=102))
                        {
                            get_address_int[i]=get_address[i]-87;
                            printf_tiny("\n\rget_address_int[%d]=%d\n\r",i,get_address_int[i]);
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag=1;
                    }
                    break;

                    default:break;
            }
        }
    }while(flag==1);

/*-------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Convert address into a number which is an integer*/
    for(i=0;i<3;i++)
    {
        address = address+((get_address_int[i])*powf(16,2-i));
    }
/*-------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Perform shifting and setting byte=1010A3A2A1(w/r) <--binary form*/
    byte = ((0xA0)|(address >> 7)&(0xfe));
    byte_2 = ((0xA0)|(address >> 7)|(0x01));

    /*Store the lower byte of the address in byte_1*/
    byte_1 = (uint8_t)address;
/*-------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Start function*/
    start();
/*-------------------------------------------------------------------------------------------------------------------------------------------*/

/*To transfer the bits sequentially on the serial terminal with MSB being transferred first
      This part of the code was referenced from (https://www.8051projects.net/wiki/I2C_Implementation_on_8051#Implementing_I2C_in_C)'s
      Tutorial section.
      EXPLANATION AND WORKING OF THIS SECTION OF THE CODE:
      1) The MSB of the byte variable is being extracted and checked if it's equal to 0 or not.If its equal to 0,then SDA=0 is being sent onto
         the serial terminal else SDA=1 is being sent onto the serial terminal.
      2) Once the first MSB has been checked,a left shift has been performed to check and perform the following operation on the next bit.
      3) Step 2 is executed until 8 bits have been checked and transmitted.
      4) Once all the bits have been checked an ACK is received to inform the MASTER that all the 8 bits were received.
    */
    printf_tiny("\n\rWrite sequence\n\r");
    for (i=0;i<8;i++)
    {
        //SCL=0;
		if((byte & 0x80) == 0)
        {
            SDA = 0;
            delay();
        }
		else
        {
            SDA = 1;
            delay();
        }
		SCL = 1;
		delay();
		SCL=0;
		byte<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*PRINT LOWER BYTE OF THE ADDRESS*/
	 SCL=0;
	 printf_tiny("\n\rLower address bytes\n\r");
	 /*Send lower byte of address*/
	 for (j=0;j<8;j++)
    {
		if((byte_1 & 0x80) == 0)
        {
            SDA = 0;
            delay();
            printf_tiny("\n\r%d\n\r",SDA);
        }
		else
        {
            SDA = 1;
            delay();
            printf_tiny("\n\r%d\n\r",SDA);
        }
		SCL = 1;
		delay();
		SCL=0;
		byte_1<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Start function*/
    printf_tiny("\n\rValue of SDA= %d \n\r",SDA);
    printf_tiny("\n\rValue of SCL= %d \n\r",SCL);
    SCL=0;
    delay();
    start();
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Write sequence which indicate the slave address,block select and the read bits*/
    printf_tiny("\n\rWrite sequence\n\r");
    for (i=0;i<8;i++)
    {
		if((byte_2 & 0x80) == 0)
        {
            SDA = 0;
            delay();
            printf_tiny("\n\r%d\n\r",SDA);
        }
		else
        {
            SDA = 1;
            delay();
            printf_tiny("\n\r%d\n\r",SDA);
        }
		SCL = 1;
		delay();
		SCL=0;
		byte_2<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

    /*Read the data stored at a particular address*/
    SDA=1;               //Relase the bus/set SDA as input
    SCL=1;
    delay();
    SCL=0;
    for(i=0;i<8;i++)
    {
        SDA=1;
        delay();
        SCL=1;
        delay();

        if(SDA==1)
        {
            z=1;
        }
        else
        {
            z=0;
        }
        dat=z;
        printf_tiny("\n\r[%d]-->%d\n\r",7-i,dat);
        bit_received |= (dat<<(7-i)) ;
        SCL=1;
        delay();
        SCL=0;
        delay();
    }
    printf_tiny("\n\rBit Received=%x\n\r",bit_received);
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*NACK function*/
    nack();
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Stop Function*/
    stop();
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              HEX DUMP FUNCTION                                                                */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void hex_dump()
{
    uint16_t first_address;
    uint16_t second_address;
    uint16_t temp_data;
    int counter;
    int flag_4=0;
    int count=0;

    printf_tiny("\n\rEnter the FIRST address between 0x00-0x7ff\n\r");
    first_address=take_address_om();
    printf_tiny("\n\rEnter the SECOND address between 0x00-0x7ff\n\r");
    second_address=take_address_om();

    if(first_address <= second_address)
    {
        flag_4=1;
        printf_tiny("\n\rValid scene\n\r");

    }
    else
    {
        printf_tiny("\n\rInvalid Entries! Please enter hex dump again\n\r");
    }

    if(flag_4 == 1)
    {
        for(counter =(first_address);counter <= (second_address); (counter)++)
        {
            if(counter==first_address)
            {
                temp_data=seq_read(first_address);
                printf_tiny("\n\r%x\t:\t%x",first_address,temp_data);
            }
            else if(count != 15)
            {
                temp_data=seq_read(counter);
                printf_tiny("\t%x",temp_data);
                count++;
            }
            else if(count == 15)
            {
                temp_data=seq_read(counter);
                printf_tiny("\n\r%x\t:\t%x",counter,temp_data);
                count=0;
            }
        }


    }
}

/*-------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              READ DATA FOR HEX DUMP                                                       */
/*-------------------------------------------------------------------------------------------------------------------------------------------*/

    /*Pass the data at the addresses*/
    uint16_t seq_read(uint16_t recv_addr)
    {
        uint16_t bit_received_1;
        uint16_t val;
        uint8_t byte_4;
        uint8_t byte_5;
        uint8_t byte_6;
        int i;
        int j;
        int x;

        /*Perform shifting and setting byte=1010A3A2A1(w/r) <--binary form*/
        byte_4 = ((0xA0)|(recv_addr >> 7)&(0xfe));
        byte_5 = ((0xA0)|(recv_addr >> 7)|(0x01));

        /*Store the lower byte of the address in byte_1*/
        byte_6 = (uint8_t)recv_addr;

/*-------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Start function*/
    start();
/*-------------------------------------------------------------------------------------------------------------------------------------------*/

/*To transfer the bits sequentially on the serial terminal with MSB being transferred first
      This part of the code was referenced from (https://www.8051projects.net/wiki/I2C_Implementation_on_8051#Implementing_I2C_in_C)'s
      Tutorial section.
      EXPLANATION AND WORKING OF THIS SECTION OF THE CODE:
      1) The MSB of the byte variable is being extracted and checked if it's equal to 0 or not.If its equal to 0,then SDA=0 is being sent onto
         the serial terminal else SDA=1 is being sent onto the serial terminal.
      2) Once the first MSB has been checked,a left shift has been performed to check and perform the following operation on the next bit.
      3) Step 2 is executed until 8 bits have been checked and transmitted.
      4) Once all the bits have been checked an ACK is received to inform the MASTER that all the 8 bits were received.
    */
    //printf_tiny("\n\rWrite sequence\n\r");
    for (i=0;i<8;i++)
    {
        //SCL=0;
		if((byte_4 & 0x80) == 0)
        {
            SDA = 0;
            delay();
        }
		else
        {
            SDA = 1;
            delay();
        }
		SCL = 1;
		delay();
		SCL=0;
		byte_4<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
     /*PRINT LOWER BYTE OF THE ADDRESS*/
	 SCL=0;
	 /*Send lower byte of address*/
	 for (j=0;j<8;j++)
    {
		if((byte_6 & 0x80) == 0)
        {
            SDA = 0;
            delay();
        }
		else
        {
            SDA = 1;
            delay();
        }
		SCL = 1;
		delay();
		SCL=0;
		byte_6<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Start function*/
    SCL=0;
    delay();
    start();
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*Write sequence which indicate the slave address,block select and the read bits*/
    for (i=0;i<8;i++)
    {
		if((byte_5 & 0x80) == 0)
        {
            SDA = 0;
            delay();
        }
		else
        {
            SDA = 1;
            delay();
        }
		SCL = 1;
		delay();
		SCL=0;
		byte_5<<=1;
	 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
 /*To check if ACK is being received by the master or not*/
 if(ack())
 {
     //printf_tiny("\n\rACK RECEIVED\n\r");
 }
 else
 {
    //printf_tiny("\n\rACK NOT RECEIVED\n\r");
 }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

    /*Read the data stored at a particular address*/
    SDA=1;               //Relase the bus/set SDA as input
    SCL=1;
    delay();
    SCL=0;
    bit_received_1=0;
    for(i=0;i<8;i++)
    {
        SDA=1;
        delay();
        SCL=1;
        delay();

        if(SDA==1)
        {
            x=1;
        }
        else
        {
            x=0;
        }
        val=x;
        bit_received_1 |= (val<<(7-i)) ;
        SCL=1;
        delay();
        SCL=0;
        delay();
    }
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
    /*ACK function*/
    ack();

    return (bit_received_1);
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
}

/*1st and 2nd address for Hex dump*/
uint16_t take_address_om()
{
    int flag_3;
    uint16_t address_om;            //to store the inputed address from the user in a single integer form
    uint8_t i;                      //counter variable
    char take_address[3];           //Get address from user
    uint16_t take_address_int[3];   //store the inputed address in int array(char array-->int array)


    address_om=0;
    /*Enter address and check validity of address*/
    do
    {
        flag_3=0;
        for(i=0;i<3;i++)
        {
            take_address[i]=getchar();
            putchar(take_address[i]);
        }

        for(i=0;i<3;i++)
        {
            switch(i)
            {
                case 0:
                    if((take_address[i]>=48) && (take_address[i]<=55))
                    {
                        take_address_int[i]= take_address[i]-48;
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag_3=1;
                    }
                    break;

                case 1:
                    if(((take_address[i]>='0') && (take_address[i]<='9')) ||  ((take_address[i]>='A') && (take_address[i]<='F')) || ((take_address[i]>='a') && (take_address[i]<='f')))
                    {
                        if((take_address[i]>=48) && (take_address[i]<=57))
                        {
                            take_address_int[i]=take_address[i]-48;
                        }
                        else if((take_address[i]>=65) && (take_address[i]<=70))
                        {
                            take_address_int[i]=take_address[i]-55;
                        }
                        else if((take_address[i]>=97) && (take_address[i]<=102))
                        {
                            take_address_int[i]=take_address[i]-87;
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag_3=1;
                    }
                break;

                case 2:
                    if(((take_address[i]>='0') && (take_address[i]<='9')) ||  ((take_address[i]>='A') && (take_address[i]<='F')) || ((take_address[i]>='a') && (take_address[i]<='f')))
                    {
                        if((take_address[i]>=48) && (take_address[i]<=57))
                        {
                            take_address_int[i]=take_address[i]-48;
                        }
                        else if((take_address[i]>=65) && (take_address[i]<=70))
                        {
                            take_address_int[i]=take_address[i]-55;
                        }
                        else if((take_address[i]>=97) && (take_address[i]<=102))
                        {
                            take_address_int[i]=take_address[i]-87;
                        }
                    }
                    else
                    {
                        printf_tiny("\n\rInvalid Input! Please enter again\n\r");
                        flag_3=1;
                    }
                    break;

                    default:break;
            }
        }
    }while(flag_3==1);

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

    /*Convert address into a number which is an integer*/
    for(i=0;i<3;i++)
    {
        address_om = address_om +((take_address_int[i])*powf(16,2-i));
    }
    return (address_om);
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                                  RESET EEPROM FUNCTION                                                        */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void eereset()
{
    uint8_t k;
    start();
    for(k=0;k<9;k++)
    {
        SDA=1;
        delay();
        SCL=1;
        delay();
        SCL=0;
        delay();
    }
    SCL=1;
    delay();
    start();
    stop();
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
//                                                              MAIN FUNCTION                                                                            */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/

void main(void)
{
    int flag=0;
    char g;
    printf_tiny("\n\rEnter the operation you want to perform\n\r");
    do{
        /*Printf statements for Menu*/
        printf_tiny("\n\rPress 1 for 'Write Byte'\n\r");
        printf_tiny("\n\rPress 2 for 'Read Byte'\n\r");
        printf_tiny("\n\rPress 3 for 'Hex Dump'\n\r");
        printf_tiny("\n\rPress 4 for 'Reset EEPROM'\n\r");
        printf_tiny("\n\rPress E for 'Exit'\n\r");

        /*To display entered characters on the serial terminal */
        g=getchar();
        putchar(g);

        /*Check for input entered and perform the corresponding operation*/
        /*Write byte operation*/
        if(g=='1')
        {
            data_write();
        }
        /* Random read operation*/
        else if(g=='2')
        {
            data_read();
        }
        /*Hex Dump*/
        else if(g=='3')
        {
            hex_dump();
        }
        else if(g=='4')
        {
            eereset();
        }
        else if((g=='e')|| (g=='E'))
        {
            break;
        }
        else
        {
            printf_tiny("\n\rInvalid input!\n\rEnter again\n\r");
            flag=0;
        }
    }while(flag==0);
    printf_tiny("\n\rEXIT!\n\rPress the RESET button to start over again!\n\r");
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              END OF PROGRAM                                                                   */
/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
