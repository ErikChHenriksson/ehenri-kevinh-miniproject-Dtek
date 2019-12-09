#include <stdint.h>
#include <pic32mx.h>
#include "miniproj.h"

int getsw(void);
int getbtns(void);

int getsw(void){
    return (PORTD & 0xf00) >> 8; // return bits 8-15 of PORTD in LSB
}

int getbtns(void){
    return (PORTD & 0xe0) >> 5;
}