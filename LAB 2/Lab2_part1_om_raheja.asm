/* Author      : Om Raheja 
 * Course      : ECEN 5613 (Embedded Systems Design)
 * Semester    : FALL 2018
 * Description : 
                
 **/

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                 START OF PROGRAM                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


      ORG 0000H 
      LJMP MAIN
      ORG 0050H
MAIN: MOV IE,#82H        ;Enable timer interrupt
      MOV TMOD,#01H      ;timer 0,mode 1
      MOV R5,#0AH        ;counter runs 10 times (0AH)
      MOV TH0,#1EH       ;Load higher register of timer
      MOV TL0,#0FDH      ;Load lower register of timer
      MOV TCON,#0001H    ;start timer
HERE: SJMP HERE
      ORG 000BH          ;Vector address of timer 0 interrupt
      CPL P1.6           ;toggle when control switches from main to isr
      DJNZ R5,ABCD
XYZ:  CPL P1.2           ;toggle led after every 0.625sec
      MOV TH0,#1EH 
      MOV TL0,#0FDH
      MOV R5,#0AH
      CPL P1.6           ;toggle Led at P1.6
      RETI               ;return from isr
ABCD: MOV TH0,#1EH 
      MOV TL0,#0FDH
      CPL P1.6
      RETI


/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                 END OF PROGRAM                                                     */
/*--------------------------------------------------------------------------------------------------------------------*/
