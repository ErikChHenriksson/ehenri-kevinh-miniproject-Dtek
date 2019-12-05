#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void);
int getbtns(void);

int getsw(void) {
    return (PORTD & 0xf00) >> 8; //return bits 8-11 as least significant bits of return value

}

int getbtns(void){
    return (PORTD & 0xe0) >> 5; //return bits 5-7 as least significant bits of return value
}