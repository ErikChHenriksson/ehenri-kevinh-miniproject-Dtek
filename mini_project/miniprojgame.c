/* This file contains the functions for game initialization, game loop and interrupts as well as their required data */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "miniproj.h" /* Declatations for the game */

#define NUM_PROJS 8
#define NUM_AST 3
#define TURNING_SPEED 5

//Score counter
int score;
//Used for interrupt
int timeoutcount = 0;
//Array showing the current state of the game
uint8_t game_state[BOUNDS_X][BOUNDS_Y];
//Player 1
float p1[6];
float p1angle;
float pcenter_x = 64;
float pcenter_y = 18.66666667;

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

uint8_t ammo = 0xff; //To fill RE0 to RE7 LEDs
uint8_t reloading = 0;
uint8_t shootingdelay = 0;

/* Resets the state of the game */
void reset_game(void)
{
  //Set score to 0
  score = 0;
  //Set gamestate to 0
  int x, y;
  for (x = 0; x < BOUNDS_X; x++)
  {
    for (y = 0; y < BOUNDS_X; y++)
    {
      game_state[x][y] = 0;
    }
  }
  //Setup player
  p1[0] = 64;
  p1[1] = 12;
  p1[2] = 61;
  p1[3] = 22;
  p1[4] = 67;
  p1[5] = 22;
  p1angle = 270;
  //Reset ammo
  ammo = 0xff;
  PORTESET = ammo;
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
  //Setup asteroids
  int k, l;
  for (k = 0; k < NUM_AST; k++)
  {
    ast[k].num_edges = 5;
    ast[k].pointarr[0] = -1;     //Set X1
    ast[k].pointarr[1] = -1 - 2; //Set Y1
    ast[k].pointarr[2] = -1 + 3; //Set X2
    ast[k].pointarr[3] = -1 - 1; //Set Y2
    ast[k].pointarr[4] = -1 + 2; //Set X3
    ast[k].pointarr[5] = -1 + 2; //Set Y3
    ast[k].pointarr[6] = -1 - 2; //Set X4
    ast[k].pointarr[7] = -1 + 2; //Set Y4
    ast[k].pointarr[8] = -1 - 3; //Set X5
    ast[k].pointarr[9] = -1 - 1; //Set Y5
    ast[k].direction = 180;
    ast[k].speed = 0.3;
    ast[k].active = 0;
    ast[k].center_x = -1;
    ast[k].center_y = -1;
    ast[k].radius = 2;
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
void game_loop(void)
{
  int buttons = getbtns();
  int switches = getsw();
  //BUTTON 4
  if ((buttons & 4))
  {
    draw_shape(3, p1, 0); //Erase old drawing of p1
    rotate(-TURNING_SPEED, pcenter_x, pcenter_y, 3, p1);
    p1angle -= TURNING_SPEED;
  }
  //BUTTON 3
  if ((buttons & 2))
  {
    draw_shape(3, p1, 0); //Erase old drawing of p1
    rotate(TURNING_SPEED, pcenter_x, pcenter_y, 3, p1);
    p1angle += TURNING_SPEED;
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
      if (move(projs[i].direction, projs[i].speed, projs[i].num_edges, projs[i].pointarr, 0, 0) == 1) //If 1, has been moved.
      {
        //Draw projectile
        draw_shape(projs[i].num_edges, projs[i].pointarr, 1);
        int j;
        for (j = 0; j < NUM_AST; j++)
        {
          //Checks if it hit any active asteroid
          if (ast[j].active == 1 && line_circle_collision(projs[i].pointarr[0], projs[i].pointarr[1], projs[i].pointarr[2], projs[i].pointarr[3], ast[j].center_x, ast[j].center_y, ast[j].radius))
          {
            //Hit! Update score
            score++;
            draw_shape(ast[j].num_edges, ast[j].pointarr, 0);
            ast[j].active = 0;
          }
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

  //MOVE ASTEROIDS
  for (i = 0; i < NUM_AST; i++)
  {
    if (ast[i].active == 1) //If active, move the asteroid
    {
      draw_shape(ast[i].num_edges, ast[i].pointarr, 0);
      rotate(1, ast[i].center_x, ast[i].center_y, ast[i].num_edges, ast[i].pointarr);
      //Move asteroid if  within screen
      if (move(ast[i].direction, ast[i].speed, ast[i].num_edges, ast[i].pointarr, &ast[i].center_x, &ast[i].center_y) == 1)
      {
        draw_shape(ast[i].num_edges, ast[i].pointarr, 1);
        //Check if there is a collision with p1
        if ((line_circle_collision(p1[0], p1[1], p1[2], p1[3], ast[i].center_x, ast[i].center_y, ast[i].radius)) || (line_circle_collision(p1[2], p1[3], p1[4], p1[5], ast[i].center_x, ast[i].center_y, ast[i].radius)) || (line_circle_collision(p1[4], p1[5], p1[0], p1[1], ast[i].center_x, ast[i].center_y, ast[i].radius)))
        {
          game_over(score);
          reset_game();
        }
      }
      else //Outside of screen. Set to inactive
      {
        ast[i].active = 0;
      }
    }
    else
    { //If unactive, spawn the asteroid somewhere else on the screen and set it to active.
      spawn_asteroid(&ast[i].radius, &ast[i].direction, &ast[i].speed, &ast[i].center_x, &ast[i].center_y, ast[i].num_edges, ast[i].pointarr);
      ast[i].active = 1;
    }
  }
  //Draw player 1
  draw_shape(3, p1, 1);
  display_update();
}
