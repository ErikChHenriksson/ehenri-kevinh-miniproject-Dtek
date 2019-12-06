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
//Player 1
float p1[] = {64, 12, 61, 22, 67, 22};

int projxpos, projypos;
int ammo = 0xff; //To fill RE0 to RE7 LEDs
int reloading = 0;

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

  TRISECLR = 0xff; //Set RE0 to RE7, LED lights, to outputs.
  PORTESET = 0xff; //Show initial ammo. Fill all LED lights.

  TMR2 = 0x0;
  T2CON = 0x70;                // reset timer,stop it and set scale to 1:256
  PR2 = (80000000 / 256) / 10; // set period register
  //The clock rate of the board is 80MHz
  // which we know from the lecture
  // so we need a prescaler division of 256 in order
  // to make the number small enough for the 16bit
  // timer
  T2CONSET = 0x8000; //Turn on timer

  IEC(0) |= 0x100;    // set T2IE to enable TMR2 interrupt
  IEC(0) |= 0x800;    // enable INT2 interrupt
  IPC(2) = 4;         // set priority for TMR2 interrupt
  enable_interrupt(); // enable interrupts globally
  return;
}

/* Interrupt Service Routine */
void user_isr(void)
{
  if (IFS(0) & 0x100)
  {
    //Our added code on top
    timeoutcount += 1;
    IFS(0) ^= 0x100; // Reset TMR2 event flags
    if (timeoutcount == 10 && reloading == 1)
    {
      reloading = 0;
      ammo = 0xff;
      PORTESET = ammo;
      timeoutcount = 0;
    }
  }
  if (IFS(0) & 0x800)
  {

    IFS(0) ^= 0x800; //Reset INT@ flag
  }
}

/* This function is called repetitively from the main program */
void labwork(void)
{
  //prime = nextprime( prime );
  //display_string( 0, itoaconv( prime ) );
  int buttons = getbtns();
  int switches = getsw();
  //BUTTON 4
  if ((buttons & 4 && (p1[2]) > 0))
  {
    draw_shape(3, p1, 0);
    p1[0]--;
    p1[2]--;
    p1[4]--;
  }
  //BUTTON 3
  if ((buttons & 2 && (p1[4]) < 127))
  {
    draw_shape(3, p1, 0);
    p1[0]++;
    p1[2]++;
    p1[4]++;
  }
  //BUTTON 2
  if ((buttons & 1) && projypos <= 0 && reloading == 0) //Attempt to fire projectile. (may only have one projectile on the screen at any time)
  {
    projxpos = p1[0];
    projypos = p1[1];
    ;

    //Update ammo
    if (ammo <= 1)
    {
      ammo = ammo >> 1;
      reloading = 1;
      timeoutcount = 0;
    }
    else
    {
      ammo = ammo >> 1;
    }
    PORTECLR = 0xff;
    PORTESET = ammo;
  }
  else if (projypos <= 0)
  {
    draw(projxpos, projxpos + 1, projypos, projypos + 1, 0);
  }
  else
  {
    //Move projectile
    draw(projxpos, projxpos + 1, projypos, projypos + 1, 0);
    projypos--;
    //Draw projectile
    draw(projxpos, projxpos + 1, projypos, projypos + 1, 1);
  }

  //Test draw line
  // create and draw a square
  float square[] = {2, 2, 32, 2, 32, 30, 2, 30};
  draw_shape(4, square, 1);
  float squarecenterx = get_center_x(4, square);
  float squarecentery = get_center_y(4, square);
  draw(squarecenterx, squarecenterx + 1, squarecentery, squarecentery + 1, 1);
  //Draw player 1
  draw_shape(3, p1, 1);
  float p1centerx = get_center_x(3, p1);
  float p1centery = get_center_y(3, p1);
  draw(p1centerx, p1centerx + 1, p1centery, p1centery + 1, 1);
  display_update();
}
