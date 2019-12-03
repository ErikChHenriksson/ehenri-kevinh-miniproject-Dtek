/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

volatile int* dispReg[] = {&PORTE, &LATE, &LATB};

int i = 0;

char textstring[] = "text, more text, and even more text!";

/* Lab-specific initialization goes here */
void labinit( void )
{
  //volatile int TRISE = *((volatile int*) 0xbf886100) & 0xff;

  TRISD |= 0xfe0;
  return;
}

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

void labwork(void) {
  

  int buttons = getbtns();
  int switches = getsw();

  if ((buttons & 4)){
    i = 0;
  }
  if ((buttons & 2)){
    i = 1;
  }
  if ((buttons & 1)){
    i = 2;
  }

  display_debug(dispReg[i]);

}