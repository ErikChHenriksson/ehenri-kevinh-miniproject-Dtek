/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

int timeoutcount = 0;
//Array chowing the current state of the game
uint8_t game_state[128][32];
//Position of player1
int p1xpos = 62;
int p1ypos = 14;
int projxpos, projypos;
int potRead = 0;
int potVal = 0;  //Mapped Value of Pot
int lastPot = 0; //last value of Pot Stored

/* Interrupt Service Routine */
void user_isr(void)
{
  if (IFS(0) & 0x100)
  { // if TM2 flag
    timeoutcount += 1;
    IFS(0) ^= 0x100; // reset TM2 flag

    if (timeoutcount == 10)
    {
      timeoutcount = 0;
    }
  }
  if (IFS(0) & 0x800)
  {                  // if INT2 flag
    IFS(0) ^= 0x100; //reset INT2 flag
  }
}

/* Lab-specific initialization goes here */
void labinit(void)
{
  int i, j;
  /* for (i = 0; i < 128; i++)
  {
    for (j = 0; j < 32; j++)
    {
      if (j == 3)
      {
        game_state[i][j] = 1;
      }
    }
  } */

  TRISD |= 0xfe0; // set bits 5-11 of PORTD to input mode

  TRISA |= 1; //Initialize PORTA0 as input    USED FOR POTENTIOMETER

  T2CON = 0x70;                // reset timer,stop it and set scale to 1:256
  PR2 = (80000000 / 256) / 10; // set period register
  TMR2 = 0x0;                  // reset timer
  T2CONSET = 0x8000;           // turn on timer

  IEC(0) |= 0x100;    // set T2IE to enable TMR2 interrupt
  IEC(0) |= 0x800;    // enable INT2 interrupt
  IPC(2) = 4;         // set priority for TMR2 interrupt
  enable_interrupt(); // enable interrupts globally
  return;
}

/* This function is called repetitively from the main program */
void labwork(void)
{
  //prime = nextprime( prime );
  //display_string( 0, itoaconv( prime ) );
  int buttons = getbtns();
  int switches = getsw();
  //BUTTON 4
  if ((buttons & 4 && (p1xpos) > 0))
  {
    erase(p1xpos, p1xpos + 5, p1ypos, p1ypos + 5);
    p1xpos--;
  }
  //BUTTON 3
  if ((buttons & 2 && (p1xpos + 5) < 128))
  {
    erase(p1xpos, p1xpos + 5, p1ypos, p1ypos + 5);
    p1xpos++;
  }
  //BUTTON 2
  if ((buttons & 1))
  {
    projxpos = p1xpos + 2;
    projypos = p1ypos - 1;
  }
  else if (projypos > 0)
  {
    //Move projectile
    erase(projxpos, projxpos + 1, projypos, projypos + 1);
    projypos--;
    projxpos++;
  }
  if (projypos == 0)
  {
    erase(projxpos, projxpos + 1, projypos, projypos + 1);
  }
  else
  {
    //Draw projectile
    draw(projxpos, projxpos + 1, projypos, projypos + 1);
  }

  //Draw player 1
  draw(p1xpos, p1xpos + 5, p1ypos, p1ypos + 5);
  //Draw projectile
  display_update();
}
