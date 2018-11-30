DESCRIPTION:

An assembly language program which shall calculate the following equation using integer division: Z = (X/(Y*4))
where, X, Y, and Z are unsigned 8-bit values.

Example: If X=0x2A and Y=0x02, then Z=0x05. (2*4 = 8 and 42 div 8 = 5)/

Preconditions:
-> Accumulator contains the dividend X. 
-> B register contains the divisor Y.

Post Conditions: 
-> Internal data memory (IRAM) location 0x20 contains the value of (Y4). 
-> IRAM location 0x21 contains the 8-bit quotient of X/(Y4) (in hexadecimal). 
-> IRAM location 0x22 contains the 8-bit remainder of X/(Y*4) (in hexadecimal). 
-> IRAM location 0x23 contains the 8-bit result Z (in hexadecimal). 
-> IRAM location 0x30 contains the error code.

Other requirements: 
-> This program must implement an algorithm that does not use the 8051 divide (DIV) or multiply (MUL) instructions. 
-> Use an 8051 shift/rotate instruction to implement the multiplication. 
-> The program must start at address 0000h. 
-> When the calculation is complete, the program jumps to a label named 'ENDLOOP' and enters an infinite loop.

Error handling: 
-> If no error occurs during program execution, the program shall write a ‘0x00 into the error code in internal memory. 
-> If the divisor is 0, then the program shall write a ‘0x01’ into the error code in internal memory and then immediately
   jump to ‘ENDLOOP’ without performing any calculations. 
-> If the calculation exceeds an 8-bit value when multiplying, then the program shall leave the least significant 8 bits 
   of the result in Z and shall write a ‘0x02’ into the error code in internal memory.

