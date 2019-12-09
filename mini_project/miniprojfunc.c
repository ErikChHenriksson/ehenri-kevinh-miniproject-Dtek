#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "miniproj.h" /* Declatations for the game */

const float PI = 3.1415926535;
#define EEPROM_write 0xa0
#define EEPROM_read 0xa1

/* Declare a helper function which is local to this file */
static void num32asc(char *s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc)
{
  int i;
  for (i = cyc; i > 0; i--)
    ;
}

/* display_debug
   A function to help debugging. Borrowed from mipslabfunc

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug(volatile int *const addr)
{
  display_string(1, "Addr");
  display_string(2, "Data");
  num32asc(&textbuffer[1][6], (int)addr);
  num32asc(&textbuffer[2][6], *addr);
  display_update();
}

uint8_t spi_send_recv(uint8_t data)
{
  while (!(SPI2STAT & 0x08))
    ;
  SPI2BUF = data;
  while (!(SPI2STAT & 1))
    ;
  return SPI2BUF;
}

void display_init(void)
{
  DISPLAY_CHANGE_TO_COMMAND_MODE;
  quicksleep(10);
  DISPLAY_ACTIVATE_VDD;
  quicksleep(1000000);

  spi_send_recv(0xAE);
  DISPLAY_ACTIVATE_RESET;
  quicksleep(10);
  DISPLAY_DO_NOT_RESET;
  quicksleep(10);

  spi_send_recv(0x8D);
  spi_send_recv(0x14);

  spi_send_recv(0xD9);
  spi_send_recv(0xF1);

  DISPLAY_ACTIVATE_VBAT;
  quicksleep(10000000);

  spi_send_recv(0xA1);
  spi_send_recv(0xC8);

  spi_send_recv(0xDA);
  spi_send_recv(0x20);

  spi_send_recv(0xAF);
}

void display_string(int line, char *s)
{
  int i;
  if (line < 0 || line >= 4)
    return;
  if (!s)
    return;

  for (i = 0; i < 16; i++)
    if (*s)
    {
      textbuffer[line][i] = *s;
      s++;
    }
    else
      textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data)
{
  int i, j;

  for (i = 0; i < 4; i++)
  {
    DISPLAY_CHANGE_TO_COMMAND_MODE;

    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(x & 0xF);
    spi_send_recv(0x10 | ((x >> 4) & 0xF));

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < BOUNDS_X; j++)
      spi_send_recv(~data[i * BOUNDS_Y + j]);
  }
}

void display_update(void)
{
  int i, j, k;
  uint8_t encodedPixels;

  for (i = 0; i < 4; i++)
  { // for each quadrant
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(0x0);
    spi_send_recv(0x10);

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < BOUNDS_X; j++)
    { // for each column

      // encode the column into 8 bits
      encodedPixels = 0;
      for (k = 0; k < 8; k++)
      {
        encodedPixels |= game_state[j][8 * i + k] << k;
      }
      spi_send_recv(encodedPixels); //send encoded column to buffer
    }
  }
}

void display_update_slow(void)
{
  int i, j, k;
  uint8_t encodedPixels;

  for (i = 0; i < 4; i++)
  { // for each quadrant
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(0x0);
    spi_send_recv(0x10);

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < BOUNDS_X; j++)
    { // for each column

      // encode the column into 8 bits
      encodedPixels = 0;
      for (k = 0; k < 8; k++)
      {
        encodedPixels |= game_state[j][8 * i + k] << k;
      }
      spi_send_recv(encodedPixels); //send encoded column to buffer
    quicksleep(25000);
    }
  }
}

void display_update_string(void)
{
  int i, j, k;
  int c;
  for (i = 0; i < 4; i++)
  {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(0x0);
    spi_send_recv(0x10);

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < 16; j++)
    {
      c = textbuffer[i][j];
      if (c & 0x80)
        continue;

      for (k = 0; k < 8; k++)
        spi_send_recv(font[c * 8 + k]);
    }
  }
}

//I2C FUNCTIONS HERE

/* Wait for I2C bus to become idle */
void i2c_idle()
{
  while (I2C1CON & 0x1F || I2C1STAT & (1 << 14))
    ; //TRSTAT
}

/* Send one byte on I2C bus, return ack/nack status of transaction */
uint8_t i2c_send(uint8_t data)
{
  i2c_idle();
  I2C1TRN = data;
  i2c_idle();
  return !(I2C1STAT & (1 << 15)); //ACKSTAT
}

/* Receive one byte from I2C bus */
uint8_t i2c_recv()
{
  i2c_idle();
  I2C1CONSET = 1 << 3; //RCEN = 1
  i2c_idle();
  I2C1STATCLR = 1 << 6; //I2COV = 0
  return I2C1RCV;
}

/* Send acknowledge conditon on the bus */
void i2c_ack()
{
  i2c_idle();
  I2C1CONCLR = 1 << 5; //ACKDT = 0
  I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send not-acknowledge conditon on the bus */
void i2c_nack()
{
  i2c_idle();
  I2C1CONSET = 1 << 5; //ACKDT = 1
  I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send start conditon on the bus */
void i2c_start()
{
  i2c_idle();
  I2C1CONSET = 1 << 0; //SEN
  i2c_idle();
}

/* Send restart conditon on the bus */
void i2c_restart()
{
  i2c_idle();
  I2C1CONSET = 1 << 1; //RSEN
  i2c_idle();
}

/* Send stop conditon on the bus */
void i2c_stop()
{
  i2c_idle();
  I2C1CONSET = 1 << 2; //PEN
  i2c_idle();
}

//END I2C FUNCTIONS

//MATH FUNCTIONS HERE
int abs(int num)
{
  return num < 0 ? -1 * num : num;
}
int round(float num)
{
  return num < 0 ? num - 0.5 : num + 0.5;
}
// cos estimation using taylor expansion
float cos(float deg)
{
  // convert to equivilant radian
  if (deg < 0)
    deg = -1 * deg;
  if (deg > 360)
    deg -= ((int)(deg / 360)) * 360; // mod 360
  float rad = deg * PI / 180;

  float res = 0;
  float term = 1;
  int k = 0;
  while (res + term != res)
  {
    res += term;
    k += 2;
    term *= -1 * rad * rad / k / (k - 1);
  }
  return res;
}

// sin estimation using taylor expansion
float sin(float deg)
{
  // convert to equilivant radian
  float sign = 1;
  if (deg < 0)
  {
    sign = -1;
    deg = -1 * deg;
  }
  if (deg > 360)
    deg -= ((int)deg / 360) * 360; // mod 360
  float rad = deg * PI / 180;

  float res = 0;
  float term = rad;
  int k = 1;
  while (res + term != res)
  {
    res += term;
    k += 2;
    term *= -1 * rad * rad / k / (k - 1);
  }
  return sign * res;
}

// generates a random number between min (inclusive) and max (exclusive)
int random_int(int min, int max)
{
  int seed = TMR2 & 0xffff;
  seed == 0 ? seed = 1 : 1;
  seed ^= (seed << 13);
  seed ^= (seed >> 7);
  seed ^= (seed << 17);
  seed %= max - min;
  seed = abs(seed);
  seed += min;
  return seed;
}

float sqroot(float square)
{
  float root = square / 3;
  int i;
  if (square <= 0)
    return 0;
  for (i = 0; i < 8; i++)
    root = (root + square / root) / 2;
  return root;
}

//END MATH FUNCTIONS

//BEGIN GENERATE FUNCTIONS
//Spawns an asteroid at either left or right side, at a random height
void spawn_asteroid(float *radius, float *direction, float *speed, float *center_x, float *center_y, int size, float *pointarr)
{
  //Randomize if it will come from the left or right
  int side = random_int(0, 2); //0 for left, 1 for right
  float oldcenter_x = *center_x;
  float oldcenter_y = *center_y;
  int i;

  if (side)
  { //Asteroid will be on the right side
    *center_x = BOUNDS_X + 2;
    *direction = 180;
  }
  else
  { //Asteroid will be on the left side
    *center_x = -2;
    *direction = 0;
  }
  *center_y = random_int(0, BOUNDS_Y);
  //Randomize speed, based on difficulty
  *speed = (float)random_int(2, 6) / 10;

  //Randomize scaling
  uint8_t inc_dec = random_int(0, 2);
  if (*radius <= 2)
  {
    inc_dec = 1;
  }
  else if (*radius >= 5)
  {
    inc_dec = 0;
  }
  int randomindex = random_int(0, 3);
  float factors[2][3] = {{0.5, 0.7, 0.9}, {1.1, 1.3, 1.5}};

  float factor = factors[inc_dec][randomindex];
  *radius *= factor;

  for (i = 0; i < size; i++)
  {
    *pointarr = (*pointarr * factor) + (*center_x - (oldcenter_x * factor));
    pointarr += 1;
    *pointarr = (*pointarr * factor) + (*center_y - (oldcenter_y * factor));
    pointarr += 1;
  }
}

//END GENERATE FUNCTIONS

//BEGIN MOVEMENT FUNCTIONS

void rotate(float angle, float xcenter, float ycenter, int size, float *pointarr)
{
  int i;
  float xcentered;
  float xnew;
  float ynew;
  float c = cos(angle);
  float s = sin(angle);

  for (i = 0; i < size; i++)
  {
    xnew = *pointarr - xcenter;
    xcentered = xnew;
    pointarr += 1;
    ynew = *pointarr - ycenter;

    xnew = (c * xnew) - (s * ynew);
    ynew = (s * xcentered) + (c * ynew);

    *pointarr = ynew + ycenter;
    pointarr -= 1;
    *pointarr = xnew + xcenter;

    pointarr += 2; // point to next coord's x
  }
}

uint8_t line_circle_collision(float xstart, float ystart, float xend, float yend, float xcircle, float ycircle, float rcircle)
{
  uint8_t projOnLine; // holds bool after proj is calculated

  // project onto line
  float xline = xend - xstart;
  float yline = yend - ystart;
  float scale = ((xcircle - xstart) * xline) + ((ycircle - ystart) * yline);
  scale /= (xline * xline) + (yline * yline);
  float xD = (scale * xline) + xstart;
  float yD = (scale * yline) + ystart;
  //printf("projection: (%f,%f)\n", xD, yD);

  float projdist = sqroot(((xcircle - xD) * (xcircle - xD)) + ((ycircle - yD) * (ycircle - yD)));
  float startdist = sqroot(((xcircle - xstart) * (xcircle - xstart)) + ((ycircle - ystart) * (ycircle - ystart)));
  float enddist = sqroot(((xcircle - xend) * (xcircle - xend)) + ((ycircle - yend) * (ycircle - yend)));
  //printf("projdist: %f\n", projdist);
  //printf("startdist: %f\n", startdist);
  //printf("enddist: %f\n", enddist);

  if (xstart < xend)
  {
    projOnLine = (xD >= xstart) & (xD <= xend);
  }
  else
  {
    projOnLine = (xD <= xstart) & (xD >= xend);
  }

  // if end points are in circle
  if (startdist <= rcircle || enddist <= rcircle)
  {
    return 1;
  }
  // if proj is in circle and in line segment
  if ((projdist <= rcircle) & projOnLine)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

uint8_t move(float direction, float speed, int size, float *pointarr, float *center_x, float *center_y)
{
  int i, outofbounds;
  int offscreen = 0;
  float motion_x = speed * cos(direction);
  float motion_y = speed * sin(direction);
  float new_val;

  *center_x += motion_x;
  *center_y += motion_y;

  for (i = 0; i < size; i++)
  {
    //Assume out of bounds
    outofbounds = 0;
    //X value
    new_val = *pointarr + motion_x;
    if (new_val < 0 || new_val > 127)
    {
      outofbounds = 1; //edge is out of bounds
    }
    *pointarr = new_val;

    pointarr += 1; //Move on to y value
    new_val = *pointarr + motion_y;
    if (new_val < 0 || new_val > 31)
    {
      outofbounds = 1;
      ;
    }
    *pointarr = new_val;
    pointarr += 1;
    offscreen += outofbounds; //Increment out of bounds counter
  }
  if (offscreen == size)
  {
    return 0; //All edges are off screen.
  }
  return 1;
}

uint8_t shape_within_bounds(int size, float *pointarr)
{

  int i;

  for (i = 0; i < size; i++)
  {
    //X value
    if (*pointarr < 0 || *pointarr > 127)
    {
      return 0;
    }

    pointarr += 1; //Move on to y value
    if (*pointarr < 0 || *pointarr > 31)
    {
      return 0;
    }
    pointarr += 1;
  }
  return 1;
}

//END MOVEMENT FUNCTIONS

//DRAW FUNCTIONS HERE

// draw pixels along line between 2 points
void draw_line(float xstart, float ystart, float xend, float yend, uint8_t color)
{
  float slope = (yend - ystart) / (xend - xstart);
  int i;
  int yapprox, xapprox;
  float y; //holds current y value across iterations
  float x; //holds current x value across iterations

  // draw start and end pixels if they are on the screen
  if (round(xstart) >= 0 & round(xstart) < BOUNDS_X & round(ystart) >= 0 & round(ystart) < BOUNDS_Y)
    game_state[round(xstart)][round(ystart)] = color;
  if (round(xend) >= 0 & round(xend) < BOUNDS_X & round(yend) >= 0 & round(yend) < BOUNDS_Y)
    game_state[round(xend)][round(yend)] = color;

  if (abs(xend - xstart) > abs(yend - ystart))
  { // more lines between x
    if (xstart < xend)
    { // start at xstart and move forward
      y = ystart;
      for (i = xstart + 1; i < xend; i++)
      {
        y += slope;
        yapprox = round(y);
        if (i >= 0 & i < BOUNDS_X & yapprox >= 0 & yapprox < BOUNDS_Y)
          game_state[i][yapprox] = color;
      }
    }
    else
    { // start at xend and move backwards
      y = yend;
      for (i = xend + 1; i < xstart; i++)
      {
        y += slope;
        yapprox = round(y);
        if (i >= 0 & i < BOUNDS_X & yapprox >= 0 & yapprox < BOUNDS_Y)
          game_state[i][yapprox] = color;
      }
    }
  }
  else
  { // more lines between y
    if (ystart < yend)
    { // start at ystart and move forwards
      x = xstart;
      for (i = ystart + 1; i < yend; i++)
      {
        x += 1 / slope;
        xapprox = round(x);
        if (xapprox >= 0 & xapprox < BOUNDS_X & i >= 0 & i < BOUNDS_Y)
          game_state[xapprox][i] = color;
      }
    }
    else
    { // start at yend and move backwards
      x = xend;
      for (i = yend + 1; i < ystart; i++)
      {
        x += 1 / slope;
        xapprox = round(x);
        if (xapprox >= 0 & xapprox < BOUNDS_X & i >= 0 & i < BOUNDS_Y)
          game_state[xapprox][i] = color;
      }
    }
  }
}

// Draws a shape defined as a list of points
// (draws a line from each point to the next in the array and finally from the last point to the first point)
void draw_shape(int size, float pointarr[], uint8_t color)
{
  int i;
  for (i = 0; i < (size * 2) - 2; i = i + 2)
  { // draw a line from each point to the next
    draw_line(pointarr[i], pointarr[i + 1], pointarr[i + 2], pointarr[i + 3], color);
  }
  draw_line(pointarr[size * 2 - 2], pointarr[size * 2 - 1], pointarr[0], pointarr[1], color); // draw line from last point to first
}

float get_center_x(int size, float pointarr[])
{
  int i;
  float center_x = 0;
  for (i = 0; i <= (size * 2) - 2; i = i + 2)
  { // draw a line from each point to the next
    center_x += pointarr[i];
  }
  return center_x / size;
}
float get_center_y(int size, float pointarr[])
{
  int i;
  float center_y = 0;
  for (i = 0; i <= (size * 2) - 2; i = i + 2)
  { // draw a line from each point to the next
    center_y += pointarr[i + 1];
  }
  return center_y / size;
}

//END DRAW FUNCTIONS

//BEGIN GAME STATE FUNCTIONS

void intro_screen(void)
{
  int buttons = getbtns();
  int x, y;
   for (x = 0; x < BOUNDS_X; x++)
  {
    for (y = 0; y < BOUNDS_X; y++)
    {
      game_state[x][y] = 0;
    }
  }
  display_update();
  for (x = 0; x < 128; x++)
  {
    for (y = 0; y < 32; y++)
    {
      game_state[x][y] = ufo[x][y];
    }
  }
  display_update_slow();
  quicksleep(3000000);
  while (!(buttons & 7)) //Wait for pressing of either BTN2, BTN3 or BTN 4
  {
    buttons = getbtns();
  }
}

void main_menu(void)
{
  int buttons = getbtns();
  //Display onto screen
  display_string(0, "CONTROLS");
  display_string(1, "");
  display_string(2, "BTN3,BTN4:ROTATE");
  display_string(3, "BTN2:     FIRE");
  display_update_string();
  quicksleep(5000000); //A small delay to prevent people from accidentaly pressing the wrong button
  //Wait for button pressing  
  while (!(buttons & 7)) //Wait for pressing of either BTN2, BTN3 or BTN 4
  {
    buttons = getbtns();
  }
}

void game_over(int score)
{

  int high_score;

  //GET HIGH SCORE FROM EEPROM BY READING
  do
  {
    i2c_start();
  } while (!i2c_send(EEPROM_write));
  i2c_send(0x00); //Send MSB of register address
  i2c_send(0x00); //Send LSB of register address
  do
  {
    i2c_start();
  } while (!i2c_send(EEPROM_read));
  high_score = i2c_recv(); //Attempt to read data
  i2c_nack();              //Send not acknowledged response
  i2c_stop();              //Send stop signal

  if (score > high_score) //New high score?
  {
    high_score = score;
    //SET HIGH SCORE IN EEPROM BY WRITING
    do
    {
      i2c_start();
    } while (!i2c_send(EEPROM_write));
    i2c_send(0x00);           //Send MSB of register address
    i2c_send(0x00);           //Send LSB of register address
    i2c_send((uint8_t)score); //Send actual data
    i2c_stop();               //Send stop signal
  }

  //Display onto screen
  display_string(0, "Game Over");
  display_string(1, "");
  display_string(2, "Score: ");
  display_string(3, "H_S: ");
  num32asc(&textbuffer[2][7], score);
  num32asc(&textbuffer[3][7], high_score);
  display_update_string();
  quicksleep(3000000);
  //Wait for button pressing
  while (!(getbtns() & 7))
  {
  }
}

//END GAME STATE FUNCTIONS

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc(char *s, int n)
{
  int i;
  for (i = 28; i >= 0; i -= 4)
    *s++ = "0123456789ABCDEF"[(n >> i) & 15];
}
