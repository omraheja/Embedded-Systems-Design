/* Author      : Om Raheja 
 * Course      : ECEN 5613 (Embedded Systems Design)
 * Semester    : FALL 2018
 * Description : An assembly language code which meets the following requirements:
<<<<<<< HEAD
 		 This program shall calculate the following equation using integer division: Z = (X/(Y*4))
		 where, X, Y, and Z are unsigned 8-bit values.
		 Example: If X=0x2A and Y=0x02, then Z=0x05. (2*4 = 8 and 42 div 8 = 5)
=======
 		  This program shall calculate the following equation using integer division: Z = (X/(Y*4))
		  where, X, Y, and Z are unsigned 8-bit values.
		  Example: If X=0x2A and Y=0x02, then Z=0x05. (2*4 = 8 and 42 div 8 = 5)
>>>>>>> 403ace840b36055057a5763b0a7e2c0a832b4724
 **/

/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                 START OF PROGRAM                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

	ORG 0H
	CLR C
	MOV 21H,#00H
	MOV 22H,#00H
	MOV 23H,#00H
	MOV 30H,#00H
	MOV R7,#00H
	MOV R2,A   
	MOV A,B    
	JZ ERROR   
	RLC A           ;Multiply by 2
	JC LOOP3
	RLC A           ;Multiply by 2
	JC LOOP2  
	MOV B,A    
	MOV A,R2   
	MOV R0,#0H 
MAIN:	SUBB A,B        ;Subtract B from A
     	JC LOOP         ;Jump if carry is generated
     	INC R0          ;Incremented R0 (This is the quotient)
     	MOV R7,A
     	SJMP MAIN    
LOOP: 	MOV 21H,R0      ;Move the quotient at address location 21H
       	MOV 22H,R7      ;Move Remainder at address location 22H
      	MOV 23H,R0      ;Move result ar address location 23H
      	MOV 30H,#00H    ;Put 00 at location 30H
      	SJMP END
LOOP2:	MOV 23H,A       ;Calculation exceeded 8 bit when multiplied by 2 and leaves least significant 8 bits
      	MOV 30H,#02H    ;For this error,write 0x02H at address 30H
      	SJMP END
LOOP3: 	CLR C
       	RLC A          ;Multiply by 2
       	MOV 23H,A      
       	MOV 30H,#02H   ;Calculation exceeds 8 bit when again miltiplied by and leaves the lease significant 8 bits
END:    SJMP END   
ERROR: 	MOV 30H,#0001H  ;IF Divisor is 0,STORE 0x01H AT ADDRESS 0x30H


/*--------------------------------------------------------------------------------------------------------------------*/
/*                                                 END OF PROGRAM                                                     */
/*--------------------------------------------------------------------------------------------------------------------*/
