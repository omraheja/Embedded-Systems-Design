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
/*                                                  GLOBAL VARIABLES                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

int i=0;                                                    //global variable required for delay

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                   MAIN FUNCTION                                                    */
/*--------------------------------------------------------------------------------------------------------------------*/

int main(void)
{
	WDT_A->CTL = (WDT_A_CTL_PW) | (WDT_A_CTL_HOLD);    //Stop Watchdog Timer

    	/*Configure GPIO*/
    	P1->DIR |= BIT0;
    	P1->OUT |= BIT0;

    	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;            // TACCR0 interrupt enabled
    	TIMER_A0->CCR[0]  = 65535;
    	TIMER_A0->CTL     = TIMER_A_CTL_SSEL__SMCLK |      // SMCLK, continuous mode
                            TIMER_A_CTL_MC__CONTINUOUS;

    	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;               // Enable sleep on exit from ISR

    	/*Ensures SLEEPONEXIT takes effect immediately*/
    	__DSB();

    	/*Enable global interrupt*/
    	__enable_irq();

    	NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    	while (1)
    	{
        	__sleep();

        	__no_operation();                          // For debugger
    	}
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                 INTERRUPT SERVICE ROUTINE                                          */
/*--------------------------------------------------------------------------------------------------------------------*/

/*Timer A0 interrupt service routine*/
void TA0_0_IRQHandler(void)
{
	TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
        i++;
        
	if(i==9)
        {
        	P1->OUT ^= BIT0;
        	i=0;
        }

        TIMER_A0->CCR[0] +=65535;                          // Add Offset to TACCR0
}


/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                      END OF PROGRAM                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
