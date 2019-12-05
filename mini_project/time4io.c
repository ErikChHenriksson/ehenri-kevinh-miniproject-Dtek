#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void);
int getbtns(void);

int getsw(void){
    return (PORTD & 0xf00) >> 8; // return bits 8-15 of PORTD in LSB
}

int getbtns(void){
    return (PORTD & 0xe0) >> 5;
}
//The potentiometer is attached to PORTA0. How to read this input? It is analog and should be able to use as input, but how?    
//There is an example in arduino here https://blog.digilentinc.com/getting-busy-with-projects-and-the-chipkit-basic-io-shield/
//Thats also where I got the map function, which I went to the documentation of arduino to see what it actually does and copy it below.
/* int getpot(void){
    map(PORTA & 1)
} */

//Supposedly can be used for mapping a value to another. Can be used for converting the 0-1023 value of analog input to another scale.
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}