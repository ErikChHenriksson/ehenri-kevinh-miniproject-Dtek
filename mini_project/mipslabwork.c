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

#define NUM_PROJS 3
#define NUM_AST 1

int timeoutcount = 0;
//Array chowing the current state of the game
uint8_t game_state[128][32];
//Player 1
float p1[] = {64, 12, 61, 22, 67, 22};
float p1angle = 270;

struct proj
{
  uint8_t num_edges;
  float pointarr[4];
  float direction;
  float speed;
  uint8_t onscreen;
} projs[NUM_PROJS];

struct asteroid
{
  uint8_t num_edges;
  float pointarr[10];
  float direction;
  float speed;
  uint8_t active;
  float radius;
  float center_x;
  float center_y;
} ast[NUM_AST];

//float staticasteroid[] = {120, 10, 125, 13, 123, 20, 117, 20, 115, 13};

uint8_t ammo = 0xff; //To fill RE0 to RE7 LEDs
uint8_t reloading = 0;
uint8_t shootingdelay = 0;

/* Lab-specific initialization goes here */
void labinit(void)
{
  //Setup projectiles
  int i, j;
  for (i = 0; i < NUM_PROJS; i++)
  {
    projs[i].num_edges = 2;
    for (j = 0; j < 4; j++)
    {
      projs[i].pointarr[j] = -1; //All projectiles start outside of the screen.
    }
    projs[i].direction = 0;
    projs[i].speed = 1;
    projs[i].onscreen = 0;
  }
  //Positions of first asteroid
  ast[1].pointarr[0] = 110;
  ast[1].pointarr[1] = 10;
  ast[1].pointarr[2] = 115;
  ast[1].pointarr[3] = 15;
  ast[1].pointarr[4] = 112;
  ast[1].pointarr[5] = 22;
  ast[1].pointarr[6] = 108;
  ast[1].pointarr[7] = 22;
  ast[1].pointarr[8] = 105;
  ast[1].pointarr[9] = 15;
  //Setup asteroids
  int k;
  for (k = 0; k < NUM_AST; k++)
  {
    ast[k].num_edges = 5;
    ast[k].direction = 180;
    ast[k].speed = 1;
    ast[k].active = 1;
    ast[k].center_x = get_center_x(ast[k].num_edges, ast[k].pointarr);
    ast[k].center_y = get_center_y(ast[k].num_edges, ast[k].pointarr);
    ast[k].radius = ast[k].center_y - ast[k].pointarr[1];
  }

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
    //For the short delay between shots
    shootingdelay = shootingdelay >> 1;
    //For reloadding:
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
    IFS(0) ^= 0x800; //Reset INT2 interrupt flag
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
  if ((buttons & 4))
  {
    draw_shape(3, p1, 0); //Erase old drawing of p1
    rotate(-2, 64, 18.666666667, 3, p1);
    p1angle -= 2;
  }
  //BUTTON 3
  if ((buttons & 2))
  {
    draw_shape(3, p1, 0); //Erase old drawing of p1
    rotate(2, 64, 18.666666667, 3, p1);
    p1angle += 2;
  }

  //BUTTON 2
  if ((buttons & 1) && shootingdelay == 0 && reloading == 0) //Attempt to fire projectile.
  {
    //Fins available bullet
    int i;
    int bulletindex = -1;
    for (i = 0; i < NUM_PROJS; i++)
    {
      if (projs[i].onscreen == 0)
      {
        bulletindex = i;
        break;
      }
    }
    if (bulletindex != -1) //Bullet found
    {
      //Shot successfully fired
      shootingdelay = 0x1;
      //Move proj to tip of p1
      projs[bulletindex].pointarr[0] = p1[0];
      projs[bulletindex].pointarr[1] = p1[1];
      projs[bulletindex].pointarr[2] = p1[0] + 1;
      projs[bulletindex].pointarr[3] = p1[1] + 1;
      projs[bulletindex].direction = p1angle;
      //projectile is on screen
      projs[bulletindex].onscreen = 1;

      //Update ammo
      ammo = ammo >> 1;
      if (ammo <= 0)
      {
        reloading = 1;
        timeoutcount = 0;
      }
      PORTECLR = 0xff;
      PORTESET = ammo;
    }
  }

  //MOVE PROJECTILES
  int i;
  for (i = 0; i < NUM_PROJS; i++)
  {
    //Move only onscreen projectiles
    if (projs[i].onscreen == 1)
    {
      //Erase old projectile
      draw_shape(projs[i].num_edges, projs[i].pointarr, 0);
      //Move projectile if projectile is going to be within bounds
      if (move(projs[i].direction, projs[i].speed, projs[i].num_edges, projs[i].pointarr) == 1) //If 1, has been moved.
      {
        //Draw projectile
        draw_shape(projs[i].num_edges, projs[i].pointarr, 1);

        if (line_circle_collision(projs[i].pointarr[0], projs[i].pointarr[1], projs[i].pointarr[2], projs[i].pointarr[3], ast[0].center_x, ast[0].center_y, ast[0].radius))
        {
          ammo = 0xff;
          PORTESET = ammo;
        }
      }
      else //move() func returned 0, which means proj would have been moved outside the screen. Move proj to default and do not draw. Set as off screen.
      {
        //Move proj off screen
        projs[i].pointarr[0] = -1;
        projs[i].pointarr[1] = -1;
        projs[i].pointarr[2] = -1;
        projs[i].pointarr[3] = -1;
        //projectile is off screen
        projs[i].onscreen = 0;
      }
    }
  }

  // MOVE ASTEROID
  //Erase

  //Move and draw if successfully moved.
  for (i = 0; i < NUM_AST; i++)
  {
    if (ast[i].active == 1)
    {
      draw_shape(ast[i].num_edges, ast[i].pointarr, 0);
      rotate(1, ast[i].center_x, ast[i].center_y, ast[i].num_edges, ast[i].pointarr);
      if (move(ast[i].direction, ast[i].speed, ast[i].num_edges, ast[i].pointarr) == 1)
      {
        draw_shape(ast[i].num_edges, ast[i].pointarr, 1);
      }
      else
      {
        ast[i].active = 0;
      }
    }
  }
  //Draw player 1
  draw_shape(3, p1, 1);
  display_update();
}
