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
MAIN: MOV IE,#82H        
      MOV TMOD,#01H
      MOV TH0,#0000H       
      MOV TL0,#0000H
      MOV DPTR,#7000H      
      MOV P0,#00H
      MOV R1,P0
      MOV R2,#80H
      MOV TCON,#0010H

HERE: MOV A,R1
      MOVX @DPTR,A
      INC R1
      CJNE R1,#07FH,HERE
      MOV R1,#00H
      SJMP HERE
      ORG 000BH
      CPL P1.7
OKR:  MOV A,R2
      MOVX @DPTR,A
      INC R2
      CJNE R2,#0FFH,EXIT
EXIT: RETI


/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                 END OF PROGRAM                                                     */
/*--------------------------------------------------------------------------------------------------------------------*/
