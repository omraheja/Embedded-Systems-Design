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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <at89c51ed2.h>
#include <malloc.h>

/* ************************************************************************************************************************* */
/*                                                MACROS & GLOBAL VARIABLES                                                  */
/* ************************************************************************************************************************* */

#define DEBUG
#ifdef DEBUG
#define DEBUGPORT(x) dataout(x);                           // generates a MOVX 0FFFFh,x where x is an 8-bit value
#else
#define DEBUGPORT(x)                                       // empty statement, nothing passed on from the preprocessor to the compiler
#endif

__xdata __at 0xefff uint8_t k;

/*Define heap size*/
#define HEAP_SIZE 3200
unsigned char xdata heap[HEAP_SIZE];

/*Global variables*/
char key;                                                  //variable to store key that is press
int buf_count=0;                                           //Total buffer count

xdata char *buffer_0=NULL;
xdata char *buffer_1=NULL;
xdata char *buffer_n[50];

/* ************************************************************************************************************************* */
/*                                                   FUNCTION PROTOTYPES                                                     */
/* ************************************************************************************************************************* */

char getchar(void);                                        //Function for getting a character
void putchar(char);                                        //Function for displaying a character
int take_input();                                          //Function for taking in input (4 digits)
int user_input();                                          //Function for taking in input (3 digits)
void dataout(uint8_t v);                                   //Function to output data for debugging purpose on Logic Analyzer
void serial_init();                                        //Function for Initializing serial communication
int user_input_del();                                      //Function for taking input to delete a buffer

/* ************************************************************************************************************************* */
/*                                                 SDCC EXTRERNAL STARTUP                                                    */
/* ************************************************************************************************************************* */

_sdcc_external_startup()
{
    AUXR |= 0x0C;
    AUXR &= 0xFD;
    return 0;
}

/* ************************************************************************************************************************* */
/*                                                   DATAOUT FUNCTION                                                        */
/* ************************************************************************************************************************* */

/*Dataout function for debugging */
void dataout(uint8_t v)
{
    k=v;
}

/* ************************************************************************************************************************* */
/*                                            UART SERIAL INITIALIZATION FUNCTION                                            */
/* ************************************************************************************************************************* */

/*Serial initialization (Taken care by paulmon)*/
void serial_init()
{
    SCON = 0X50;
    TMOD = 0X20;
    TH1 = 0X0FD;
    TR1 = 1;
    TI = 1;
}

/* ************************************************************************************************************************* */
/*                                                    PUTCHAR FUNCTION                                                       */
/* ************************************************************************************************************************* */

void putchar(char c)
{
    while(!TI);
    SBUF=c;
    TI=0;
}

/* ************************************************************************************************************************* */
/*                                                    GETCHAR FUNCTION                                                       */
/* ************************************************************************************************************************* */

char getchar ()
{
    while (!RI);                                           // compare asm code generated for these three lines
	RI = 0;			                                       // clear RI flag
	return SBUF;  	                                       // return character from SBUF
}

/* ************************************************************************************************************************* */
/*                                                   TAKE INPUT FUNCTION                                                     */
/* ************************************************************************************************************************* */

/*Take input from user(4 digits) to select buffer size between 16 and 3200*/
int take_input()
{

    int flag=0;
    int next=0;

    /* variables to store intermediate result for ASCII to int conversion */
    unsigned int a=0;
    unsigned int b=0;
    unsigned int c=0;
    unsigned int d=0;
    unsigned int num=0;

    do{
        flag=0;
        next=0;
        printf_tiny("Enter a buffer size between 16 and 3200 which is divisible by 16\n\r");

        /*Take input from user*/
        a=getchar();
        putchar(a);
        b=getchar();
        putchar(b);
        c=getchar();
        putchar(c);
        d=getchar();
        putchar(d);

        /*Conversion from ASCII to INTEGER*/
        num=(((a-48)*1000) + ((b-48)*100) + ((c-48)*10) + ((d-48)*1)) ;

        /*Check if number entered is between 16 and 3200 and is divisible by 16*/
            if((num>=16) && (num<=3200) && ((num%16)==0))
            {
                next=1;
                flag=0;
            }
            else
            {
                printf_tiny("\n\rInvalid input\n\r");
                flag=1;
            }
    }while(flag==1);

    return num;
}

/* ************************************************************************************************************************* */
/*                                                    USER INPUT FUNCTION                                                    */
/* ************************************************************************************************************************* */

/*Function for taking input from user (3 digits) to select new buffer size between 30 and 300*/
int user_input()
{

    int flag=0;
    int next=0;


    /* variables to store intermediate result for ASCII to int conversion */
    unsigned int x=0;
    unsigned int y=0;
    unsigned int z=0;
    unsigned int num1=0;

    do{
        flag=0;
        next=0;
        printf_tiny("\n\rEnter a buffer size between 30 and 300\n\r");

        /*Take input from user*/
        x=getchar();
        putchar(x);
        y=getchar();
        putchar(y);
        z=getchar();
        putchar(z);

        /*Conversion from ASCII to INTEGER*/
        num1=(((x-48)*100) + ((y-48)*10) + ((z-48)*1)) ;


        /*Check if number entered is between 16 and 3200 and is divisible by 16*/
        if((num1>=30) && (num1<=400))
        {
            next=1;
            flag=0;
        }
        else
        {
            printf_tiny("\n\rInvalid input\n\r");
            flag=1;
        }
    }while(flag==1);

    return num1;
}

/* ************************************************************************************************************************* */
/*                                              USER INPUT DELETE FUNCTION                                                   */
/* ************************************************************************************************************************* */

/*Function for taking input to delete a buffer*/
int user_input_del()
{
    unsigned int q=0;
    unsigned int w=0;
    unsigned int num3=0;

    printf_tiny("\n\rEnter buffer number which you want to delete\n\r");

    /*Take input from user*/
    q=getchar();
    putchar(q);
    w=getchar();
    putchar(w);

    /*Conversion from ASCII to INTEGER*/
    num3=(((q-48)*10) + ((w-48)*1));
    return num3;
}

/* ************************************************************************************************************************* */
/*                                                      MAIN FUNCTION                                                        */
/* ************************************************************************************************************************* */

void main()
{
    char input_char;                                         //For + - = ? @ functionality
    unsigned int number=0;                                   //For storing buffer size for buffer 0 and buffer 1
    unsigned int number2=0;                                  //For storing buffer size for additional buffers
    unsigned int num_del;                                    //stores a particulat buffer you want to delete
    unsigned int total_char=0;                               //Total number of char received
    unsigned int storage_char=0;                             //Total number of storage char received
    unsigned int buff_0_count;                               //to display the contents present in buffer 0 at any moment
    unsigned int total_buff_count;                           //Total buffer count
    unsigned int buff_size[50];                              //array for storing size of each buffer
    uint16_t Buffer_0temp;                                   //stores address of buffer 0
    uint16_t Buffer_1temp;                                   //stores address of buffer 1
    int count=0;                                             //index for buffer_0 so that data can be saved at locations ahead of base address
    int i=0;                                                 //to keep count of thr total number of buffers
    int t=2;                                                 //number of new buffer example: t=2 refers to buffer 2
    int m=0;                                                 //counter variable

    init_dynamic_memory((MEMHEADER xdata *)heap, HEAP_SIZE);
    DEBUGPORT(4);

        do{
            number=take_input();

            /*Allocaate memory to buffer 0*/
            buffer_0=(xdata char *)malloc(number);
            Buffer_0temp=(uint16_t)buffer_0;               //store buffer 0's address in Buffer_0temp

            /*Check if memory is successfully allocated*/
            if((buffer_0)!=0)
            {
                printf_tiny("\n\rBUFFER_0 :MEMORY SUCCESSFULLY ALLOCATED!\n\r");
                printf_tiny("\n\rStarting Address of buffer 0>>>0x%x\n\r",Buffer_0temp);
                buff_size[0]=number;

                /*Allocate memory to buffer 1*/
                buffer_1=(xdata char *)malloc(number);
                Buffer_1temp=(uint16_t)buffer_1;           //store buffer 1's address in Buffer_0temp

                /*Check if memory is successfully allocated*/
                if((buffer_1)!=0)
                {
                    printf_tiny("\n\rBUFFER_1 :MEMORY SUCCESSFULLY ALLOCATED!\n\r");
                    printf_tiny("\n\rStarting Address of buffer 1>>>0x%x\n\r",Buffer_1temp);
                    buff_size[1]=number;
                }
                else
                {
                    printf_tiny("\n\rBUFFER_1 :MEMORY ALLOCATION FAILED!\n\r");

                    /*Free buffer 0 and 1 if memory allocation for buffer 1 fails*/
                    free(buffer_0);
                    free(buffer_1);
                }
            }
            else
            {
                printf_tiny("BUFFER_0 :MEMORY ALLOCATION FAILED!\n\r");
                free(buffer_0);
            }
        }while(((buffer_0)==(NULL)) || ((buffer_1)==(NULL)));

/* ************************************************************************************************************************* */
        while(1)
        {
                printf_tiny("\n\rEnter the character\n\r");

                /*Take input for storage*/
                input_char=getchar();
                total_char++;
                putchar(input_char);
                printf_tiny("\n\r\n\r");

                if(((input_char)>='a') && (((input_char)<='z') && (count<number)))
                {
                    storage_char++;
                    *((buffer_0)+count)=input_char;
                    count++;
                }
                else if(((count) == (number)) && (input_char != '+') && (input_char != '-') && (input_char != '?') && (input_char != '@') && (input_char != '='))
                {
                    putchar(input_char);
                    printf_tiny("\n\rBUFFER 0 IS FULL\n\r");
                }
                else if(input_char == '+')
                {
                    printf_tiny("\n\r***************Allocate a new Buffer****************************\n\r");
                    printf_tiny("\n\rEnter the buffer size between 30 and 300\n\r");
                    printf_tiny("\n\rEnter 0xx for 2 digit numbers\n\rEnter xxx for 3 digit numbers");
                    number2=user_input();

                    buffer_n[i]=(xdata char *)malloc(number2);

                    if(buffer_n[i]==NULL)
                    {
                        printf_tiny("\n\rMemory Allocation for Buffer_%d Failed\n\rPress '+' to Try again\n\r",i+2);
                    }
                    else
                    {
                        printf_tiny("\n\rMemory Allocation Successful for Buffer_%d\n\r",(i+2));
                        printf_tiny("\n\rBuffer_%d allocated of size %d\n\r",(i+2),number2);
                        printf_tiny("\n\rAddress of buffer_%d = 0x%x\n\r",(i+2),(uint16_t)(buffer_n[i]));
                        buff_size[t]=number2;              //store size of newly allocated buffer to buff_size array
                        t++;
                        i++;
                    }
                }
                else if(input_char == '-')
                {
                    printf_tiny("\n\rEnter a valid buffer number\n\r");
                    printf_tiny("\n\rEnter 0x for 1 digit numbers\n\rEnter xx for 2 digit numbers\n\r");
                    num_del=user_input_del();

                    if(num_del == 0)
                    {
                        printf_tiny("\n\rBuffer 0 cannot be deleted!\n\r");
                    }
                    else if(num_del == 1)
                    {
                        free(buffer_1);
                        printf_tiny("\n\rBuffer 1>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d\n\r",(uint16_t)buffer_1);
                        buffer_1 = 0;
                        printf_tiny("Buffer 1 deleted");
                    }
                    else if(num_del>1 && num_del <=(i+2))
                    {
                        printf_tiny("\n\rDeleting buffer_%d.........\n\r",num_del);
                        free(buffer_n[num_del-2]);
                        printf_tiny("\n\rBuffer %d  Freed\n\r",num_del);
                        buffer_n[num_del-2]=0;
                    }
                    else
                    {
                        printf_tiny("\n\rEnter a valid buffer number\n\rPress '-' to delete any valid buffer\n\r");
                    }
                }

                else if(input_char == '?')
                {
                    printf_tiny("************************ HEAP REPORT ************************");

                    printf_tiny("\n\rBUFFER 0\n\r");
                    printf_tiny("\n\rStart Address>>>>>>>>>>>>>>>>>>0x%x\n\r",Buffer_0temp);
                    printf_tiny("\n\rEnding Address>>>>>>>>>>>>>>>>0x%x\n\r",(Buffer_0temp)+(number));
                    printf_tiny("\n\rBuffer Size>>>>>>>>>>>>>>>>>>>>%d\n\r",number);
                    printf_tiny("\n\rStorage characters in buffer>>>%d\n\r",storage_char);
                    printf_tiny("\n\rFree Spaces in buffer>>>>>>>>>>%d\n\r",(number-storage_char));
                    printf_tiny("\n\r*********************************************************\n\r");

                    if(buffer_1 != 0)
                    {
                        printf_tiny("\n\rBUFFER 1\n\r");
                        printf_tiny("\n\rStart Address>>>>>>>>>>>>>>>>>>0x%x\n\r",Buffer_1temp);
                        printf_tiny("\n\rEnding Address>>>>>>>>>>>>>>>>>0x%x\n\r",Buffer_1temp+number);
                        printf_tiny("\n\rBuffer Size>>>>>>>>>>>>>>>>>>>>%d\n\r",number);
                        printf_tiny("\n\rStorage characters in buffer>>>0\n\r");
                        printf_tiny("\n\rFree Spaces in buffer>>>>>>>>>>%d\n\r",(number));
                        printf_tiny("\n\r*********************************************************\n\r");
                    }

                    for(m=2;m<=(i+2);m++)
                    {
                        if(buffer_n[m-2]!=0)
                        {
                            printf_tiny("\n\r*********************************************************\n\r");
                            printf_tiny("\n\rBUFFER %d\n\r",m);
                            printf_tiny("\n\rStart Address>>>>>>>>>>>>>>>>>>0x%x\n\r",(uint16_t)buffer_n[m-2]);
                            printf_tiny("\n\rEnding Address>>>>>>>>>>>>>>>>>0x%x\n\r",(uint16_t)(buffer_n[m-2])+(uint16_t)(buff_size[m]));
                            printf_tiny("\n\rBuffer Size>>>>>>>>>>>>>>>>>>>>%d\n\r",buff_size[m]);
                            printf_tiny("\n\rStorage characters in buffer>>>0\n\r");
                            printf_tiny("\n\rFree Spaces in buffer>>>>>>>>>>%d\n\r",(buff_size[m]));
                        }
                    }
                    printf_tiny("\n\r*********************************************************\n\r");
                    printf_tiny("\n\rNumber of storage characters = %d\n\r",storage_char);
                    printf_tiny("\n\rTotal number of characters received = %d\n\r",total_char);
                    printf_tiny("\n\rTotal number of buffers that were allocated since the start of the program = %d\n\r",(i+2));
                    printf_tiny("\n\rContents in Buffer 0\n\r");

                    for((buff_0_count)=0;(buff_0_count)<storage_char;(buff_0_count)++)
                    {
                        if(buff_0_count==0)
                        {
                            printf_tiny("0x%x>> %d",(uint16_t)(buffer_0+buff_0_count),*((buffer_0)+(buff_0_count)));
                        }
                        else if((buff_0_count%16)!=0)
                        {
                            printf_tiny(" %d",*((buffer_0)+(buff_0_count)));
                        }
                        else if(buff_0_count%16==0)
                        {
                            printf_tiny("\n\r0x%x>> %d",(uint16_t)(buffer_0+buff_0_count),*((buffer_0)+(buff_0_count)));
                        }
                    }

                    printf_tiny("\n\rTotal storage characters stored since last '?' = %d\n\r",storage_char);
                    storage_char=0;
                    count=0;
                    buffer_0=Buffer_0temp;
                }
                else if(input_char == '=')
                {
                    printf_tiny("\n\rContents in Buffer 0\n\r");

                    for((buff_0_count)=0;(buff_0_count)<storage_char;(buff_0_count)++)
                    {
                        if(buff_0_count==0)
                        {
                            printf_tiny("0x%x>> %x",(uint16_t)(buffer_0+buff_0_count),*((buffer_0)+(buff_0_count)));
                        }
                        else if((buff_0_count%16)!=0)
                        {
                            printf_tiny(" %x",*((buffer_0)+(buff_0_count)));
                        }
                        else if(buff_0_count%16==0)
                        {
                            printf_tiny("\n\r0x%x>> %x",(uint16_t)(buffer_0+buff_0_count),*((buffer_0)+(buff_0_count)));
                        }
                    }
                }
                else if(input_char == '&')
                {
                    printf_tiny("\n\rContents in heap\n\r");
                    for((buff_0_count)=0;(buff_0_count)<=256;(buff_0_count)++)
                    {
                        if(buff_0_count==0)
                        {
                            printf_tiny("0x%x>> %x",(uint16_t)(buffer_0+buff_0_count),*((buffer_0)+(buff_0_count)));
                        }
                        else if((buff_0_count%16)!=0)
                        {
                            printf_tiny(" %x",*((buffer_0)+(buff_0_count)));
                        }
                        else if(buff_0_count%16==0)
                        {
                            printf_tiny("\n\r0x%x>> %x",(uint16_t)(buffer_0+buff_0_count),*((buffer_0)+(buff_0_count)));
                        }
                    }
                }
                else if (input_char == '@')
                {
                    printf_tiny("\n\rFreeing Buffer 0.........\n\r");
                    free(buffer_0);
                    printf_tiny("\n\rBuffer 1 freed\n\r");

                    printf_tiny("\n\rFreeing Buffer 1......... \n\r");
                    free(buffer_1);
                    printf_tiny("\n\rBuffer 1 freed\n\r");

                    for(total_buff_count=2;(total_buff_count)<(i+2);(total_buff_count)++)
                    {
                        printf_tiny("\n\rFreeing buffer_%d.......\n\r",total_buff_count);
                        free(buffer_n[total_buff_count]);
                        printf_tiny("\n\rBuffer %d freed \n\r",total_buff_count);
                    }
                    main();
                }
        }
}

/* ************************************************************************************************************************* */
/*                                                   END OF PROGRAM                                                          */
/* ************************************************************************************************************************* */
