/* Declare global constants */

#define BOUNDS_X 128
#define BOUNDS_Y 32

/* Declare display-related functions from miniprojfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
void display_update_slow(void);
void display_update_string(void);
uint8_t spi_send_recv(uint8_t data);
/* I2C Functions */
void i2c_idle();
uint8_t i2c_send(uint8_t data);
uint8_t i2c_recv();
void i2c_ack();
void i2c_nack();
void i2c_start();
void i2c_restart();
void i2c_stop();

/* Drawing functions */
void draw_line(float xstart, float xend, float ystart, float yend, uint8_t color);
void draw_shape(int size, float pointarr[], uint8_t color);

void game_loop(void);
void quicksleep(int cyc);
/* Gamestate functions */
void intro_screen(void);
void main_menu(void);
void game_over(int score);
/* Math functions */
extern const float PI;
int random_int(int min, int max);
int abs(int num);
int round(float num);
float cos(float deg);
float sin(float deg);
float sqroot(float square);
/* Lifespan functions */
void spawn_asteroid(float *radius, float *direction, float *speed, float *center_y, float *center_x, int size, float *pointarr);
/* Movement functions */
void rotate(float angle, float xcenter, float ycenter, int size, float *pointarr);
uint8_t line_circle_collision(float xstart, float ystart, float xend, float yend, float xcircle, float ycircle, float rcircle);
uint8_t move(float angle, float magnitude, int size, float *pointarr, float *center_x, float *center_y);
uint8_t shape_within_bounds(int size, float *pointarr);
float get_center_x(int size, float pointarr[]);
float get_center_y(int size, float pointarr[]);

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug(volatile int *const addr);

/* Declare bitmap array containing font */
extern const uint8_t const font[128 * 8];
/* Declare bitmap array containing ufo */
extern const uint8_t const ufo[128][32];
/* Declare text buffer for display output */
extern char textbuffer[4][16];
extern uint8_t game_state[BOUNDS_X][BOUNDS_Y];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string(char *, int);
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);
