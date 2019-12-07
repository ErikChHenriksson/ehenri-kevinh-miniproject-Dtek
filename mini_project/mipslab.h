/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

void draw(float xstart, float xend, float ystart, float yend, uint8_t color);
void draw_line(float xstart, float xend, float ystart, float yend, uint8_t color);
void draw_shape(int size, float pointarr[], uint8_t color);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void labwork(void);
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );

extern const float PI;
int random_int(int min, int max);
int abs(int num);
int round(float num);
float cos(float deg);
float sin(float deg);
float sqroot(float square);
void rotate(float angle, float xcenter, float ycenter, int size, float* pointarr);
uint8_t line_circle_collision(float xstart, float ystart, float xend, float yend, float xcircle, float ycircle, float rcircle);
uint8_t move(float angle, float magnitude, int size, float* pointarr);
uint8_t shape_within_bounds(int size, float* pointarr);
int get_center_x(int size, float pointarr[]);
int get_center_y(int size, float pointarr[]);



/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];
extern uint8_t game_state[128][32];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);


