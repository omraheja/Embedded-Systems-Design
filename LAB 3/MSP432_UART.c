/* Author Name : Om Raheja
 * Date        : 20th October 2018
 * Course      : ECEN 5613 Embedded System Design
 * Project     : Interrupt driven UART for TI-MSP432(ARM CORTEX M4F)
 *
 *--COPYRIGHT--,BSD_EX
 *Copyright (c) 2014, Texas Instruments Incorporated
 *All rights reserved.
 *
 *This code has been referred from the "Resource Explorer" in the Code
 *Composer Studio.The code was altered and manipulated for learning and
 *educational purpose.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions
 *are met:
 *
 *Redistributions of source code must retain the above copyright
 *notice, this list of conditions and the following disclaimer.
 *
 *Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the following disclaimer in the
 *documentation and/or other materials provided with the distribution.
 *
 *Neither the name of Texas Instruments Incorporated nor the names of
 *its contributors may be used to endorse or promote products derived
 *from this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP432 CODE EXAMPLE DISCLAIMER
 *
 * MSP432 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP432P401 Demo - eUSCI_A0 UART echo at 9600 baud using BRCLK = 12MHz
//
//  Description: This demo echoes back characters received via a PC serial port.
//  SMCLK/ DCO is used as a clock source and the device is put in LPM0
//  The auto-clock enable feature is used by the eUSCI and SMCLK is turned off
//  when the UART is idle and turned on when a receive edge is detected.
//  Note that level shifter hardware is needed to shift between RS232 and MSP
//  voltage levels.
//
//  The example code shows proper initialization of registers
//  and interrupts to receive and transmit data.
//
//                MSP432P401
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |       P7.7/TA1.1|----> CCR1 - 60% PWM
//            |     P1.3/UCA0TXD|----> PC (echo)
//            |     P1.2/UCA0RXD|<---- PC
//            |                 |
//
//   Texas Instruments Inc.
//   Built with CCSv6.1, IAR, Keil, GCC

/* ************************************************************************************************************************* */
/*                                                 START OF PROGRAM                                                          */
/* ************************************************************************************************************************* */

/* ************************************************************************************************************************* */
/*                                             C STANDARD LIBRARY HEADERS                                                    */
/* ************************************************************************************************************************* */

/*Standard Library Headers*/
#include "msp.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* ************************************************************************************************************************* */
/*                                                 GLOBAL VARIABLES                                                          */
/* ************************************************************************************************************************* */

/*Global variables*/
    volatile long temp;                 //for taking digital values after A to D conversion
    volatile float IntDegF;             //stores the temperature in Fahrenheit
    volatile float IntDegC;             //stored the temperature in Celsius
    int flag=0;                         //flag
    int w=0;                            //control variable used in a for loop to generate delay
    char array[2];                      //array to store the characters-->later converted to integer
    int number;                         //integer number stored in this variable after char to int conversion
    char cel[20];                       //Temperature in celcius stored in this array after float to string conversion
    char frh[20];                       ////Temperature in Fahrenheit stored in this array after float to string conversion


/* ************************************************************************************************************************* */
/*                                                    PUTSTR FUNCTION                                                        */
/* ************************************************************************************************************************* */

/*Put string function*/
void myputstr(char *ptr)
{
  while(*ptr)
  {
      while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));                    // Check if the TX buffer is empty first
      EUSCI_A0->TXBUF = *ptr++;                                       //Echo the received character on the serial terminal
  }
}

/* ************************************************************************************************************************* */
/*                                             PULSE WIDTH MODULATION FUNCTION                                               */
/* ************************************************************************************************************************* */

/*Pulse width modulated signal*/
void pwm()
{
    myputstr(array);
    TIMER_A1->CCR[0] = 1000 - 1;                    // PWM Period
    TIMER_A1->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;     // CCR1 reset/set
    TIMER_A1->CCR[1] = number;                      // CCR1 PWM duty cycle
    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK |       // SMCLK
                    TIMER_A_CTL_MC__UP |            // Up mode
                    TIMER_A_CTL_CLR;                // Clear TAR
 }
 

/* ************************************************************************************************************************* */
/*                                                       MAIN FUNCTION                                                       */
/* ************************************************************************************************************************* */

/*Main function*/
int main(void)
{
    /*Variables to store the ADC temperature reference calibration value*/
    int32_t adcRefTempCal_1_2v_30;
    int32_t adcRefTempCal_1_2v_85;

    /*Stop watchdog timer*/
    WDT_A->CTL = WDT_A_CTL_PW |
            WDT_A_CTL_HOLD;

    /* Read the ADC temperature reference calibration value */
    adcRefTempCal_1_2v_30 = TLV->ADC14_REF1P2V_TS30C;
    adcRefTempCal_1_2v_85 = TLV->ADC14_REF1P2V_TS85C;

    /* Initialize the shared reference module */
    /* By default, REFMSTR=1 => REFCTL is used to configure the internal reference */
           while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);            // If ref generator busy, WAIT
           REF_A->CTL0 |= REF_A_CTL0_VSEL_0 |                  // Enable internal 1.2V reference
                   REF_A_CTL0_ON;                              // Turn reference on

           REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;                   // Enable temperature sensor


   /* Configure ADC - Pulse sample mode; ADC14_CTL0_SC trigger */
               ADC14->CTL0 |= ADC14_CTL0_SHT0_6 |              // ADC ON,temperature sample period>5us
                       ADC14_CTL0_ON |
                       ADC14_CTL0_SHP;
               ADC14->CTL1 |= ADC14_CTL1_TCMAP;               // Enable internal temperature sensor
               ADC14->MCTL[0] = ADC14_MCTLN_VRSEL_1 |         // ADC input ch A22 => temp sense
                       ADC14_MCTLN_INCH_22;
               ADC14->IER0 = 0x0001;                          // ADC_IFG upon conv result-ADCMEM0

   /* Wait for reference generator to settle */
      while(!(REF_A->CTL0 & REF_A_CTL0_GENRDY));
                   ADC14->CTL0 |= ADC14_CTL0_ENC;


    // Configure GPIO
    P7->DIR |= BIT6 | BIT7;                                   // P7.7 set TA1.1~2
    P7->SEL0 |= BIT6 | BIT7;
    P7->SEL1 &= ~(BIT6 | BIT7);

    /*CS module configuration*/
    CS->KEY = CS_KEY_VAL;                                    // Unlock CS module for register access
    CS->CTL0 = 0;                                            // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;                            // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |                              // Select ACLK = REFO
            CS_CTL1_SELS_3 |                                 // SMCLK = DCO
            CS_CTL1_SELM_3;                                  // MCLK = DCO
    CS->KEY = 0;                                             // Lock CS module from unintended accesses

    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                                 // set 2-UART pin as secondary function

    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;                  // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST |                  // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;                       // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A0->BRW = 78;                                      // 12000000/16/9600
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;                 // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;                     // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;                         // Enable USCI_A0 RX interrupt

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIA0 interrupt/ADC interrupt in NVIC module
    NVIC->ISER[0] |= (1 << ((EUSCIA0_IRQn) & 31)) | (1 << ((ADC14_IRQn) & 31));

    //Sampling and conversion start
    ADC14->CTL0 |= ADC14_CTL0_SC;
    int k;
    while(1)
      {
            ADC14->CTL0 |= ADC14_CTL0_SC;       // Sampling and conversion start
            for(k =0; k<10000;k++);             //delay

            /*Calculate temperature in Celsius*/
            IntDegC = (((float) temp - adcRefTempCal_1_2v_30) * (85 - 30)) /
                    (adcRefTempCal_1_2v_85 - adcRefTempCal_1_2v_30) + 30.0f;

            /* Temperature in Fahrenheit */
            IntDegF = ((9 * IntDegC) / 5) + 32;
     }
}


/* ************************************************************************************************************************* */
/*                                             INTERRUPT SERVICE ROUTINE FOR UART                                            */
/* ************************************************************************************************************************* */

/* UART interrupt service routine */
void EUSCIA0_IRQHandler(void)
{

    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        // Check if the TX buffer is empty first
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

        /*Increase the duty cycle and toggle led at port 1*/
        if(EUSCI_A0->RXBUF =='+')
        {
            P1DIR = 0x01;
            P1OUT ^= 0x01;
            number=number+5;
            pwm();
        }

        /*Increase the duty cycle and toggle led at port 2*/
        if(EUSCI_A0->RXBUF =='-')
        {
            P2DIR |= 0x01;
            P2OUT ^= 0x01;
            number=number-5;
            pwm();
        }

        if(flag==1)
        {
            array[w]=EUSCI_A0->RXBUF;
            w++;

            if(w==2)
            {
               number=((((array[0]-48)*(10))+((array[1]-48)*1))*10);
               flag=0;
               pwm();

            }
            else
            {
                EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
                EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
            }
        }

        /*If the received character is P then display the duty cycle on the serial terminal*/
        else if((EUSCI_A0->RXBUF =='p')|| (EUSCI_A0->RXBUF =='P'))
        {
            myputstr("\n\rEnter duty cycle=");
            flag=1;
            EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
            EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
        }
        else if((EUSCI_A0->RXBUF =='t')|| (EUSCI_A0->RXBUF =='T'))
        {
            sprintf(cel,"\n\rTemperature(C)-->%f",IntDegC);
            myputstr(cel);
            sprintf(frh,"Temperature(F)-->%f",IntDegF);
            myputstr(frh);
        }

        else if((EUSCI_A0->RXBUF !='p') && (EUSCI_A0->RXBUF !='P') && (EUSCI_A0->RXBUF !='t') && (EUSCI_A0->RXBUF !='T'))
        {
            // Echo the received character back
            EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
        }

    }
}

/* ************************************************************************************************************************* */
/*                                        INTERRUPT SERVICE ROUTINE FOR ADC                                                  */
/* ************************************************************************************************************************* */


/* ADC14 interrupt service routine */
void ADC14_IRQHandler(void)
{
    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0)
    {
        temp = ADC14->MEM[0];
    }
}

/* ************************************************************************************************************************* */
/*                                                   END OF PROGRAM                                                          */
/* ************************************************************************************************************************* */
