/* Author      : Om Raheja 
 * Course      : ECEN 5613 (Embedded Systems Design)
 * Semester    : FALL 2018
 * Description : 
                
 **/

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                 START OF PROGRAM                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                   HEADER FILES                                                     */
/*--------------------------------------------------------------------------------------------------------------------*/

#include "ti/devices/msp432p4xx/inc/msp.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                   MAIN FUNCTION                                                    */
/*--------------------------------------------------------------------------------------------------------------------*/

int main(void)
{
    //Hold the watchdog
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // Configuring P1.0 as output and P1.4 (switch) as input with pull-up
    // resistor. Rest of pins are configured as output low.
    // Notice intentional '=' assignment since all P1 pins are being
    // deliberately configured
    P1->DIR = ~(uint8_t) BIT4;
    P1->OUT = BIT4;
    P1->REN = BIT4;                                        // Enable pull-up resistor (P1.4 output high)
    P1->SEL0 = 0;
    P1->SEL1 = 0;
    P1->IES = BIT4;                                        // Interrupt on high-to-low transition
    P1->IFG = 0;                                           // Clear all P1 interrupt flags
    P1->IE = BIT4;                                         // Enable interrupt for P1.4


    // Enable Port 1 interrupt on the NVIC
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);

    // Terminate all remaining pins on the device
    P2->DIR |= 0xFF; P2->OUT = BIT1;
    P3->DIR |= 0xFF; P3->OUT = 0;
    P4->DIR |= 0xFF; P4->OUT = 0;
    P5->DIR |= 0xFF; P5->OUT = 0;
    P6->DIR |= 0xFF; P6->OUT = 0;
    P7->DIR |= 0xFF; P7->OUT = 0;
    P8->DIR |= 0xFF; P8->OUT = 0;
    P9->DIR |= 0xFF; P9->OUT = 0;
    P10->DIR |= 0xFF; P10->OUT = 0;

    // Configure Port J
    PJ->DIR |= (BIT0| BIT1 | BIT2 | BIT3);
    PJ->OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);

    // Enable PCM rude mode, which allows to device to enter LPM3 without waiting for peripherals
    PCM->CTL1 = PCM_CTL0_KEY_VAL | PCM_CTL1_FORCE_LPM_ENTRY;

    // Enable global interrupt
    __enable_irq();

    // Setting the sleep deep bit
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);

    // Do not wake up on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    // Ensures SLEEPONEXIT takes effect immediately
    __DSB();

    // Go to LPM4
    __sleep();
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                             INTERRUPT SERVICE ROUTINE                                              */
/*--------------------------------------------------------------------------------------------------------------------*/

/*Port1 ISR*/
void PORT1_IRQHandler(void)
{
    volatile uint32_t i;

    // Toggling the output on the LED
    if((P1->IFG & BIT4))
    {
        P2->OUT ^=(0x06);
    }

    // Delay for switch debounce
    for(i = 0; i < 10000; i++)

    P1->IFG &= ~BIT4;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                   END OF PROGRAM                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

