#include <stdio.h>

const float PI = 3.1415926535;

struct Ship{
    float xcenter;
    float ycenter;
    float points[6];
    float direction;
};

struct Ship p1;

void lab_init(){
    p1.points[0] = 1;
    p1.points[1] = 2;
    p1.xcenter = get_center_x(3, p1.points);
}



__uint8_t scene[32][16];

// prints scene model
void print_game(){
    int x, y;
    for(x=0; x<16; x++){
        for(y=0; y<32; y++){
            printf("%u ", scene[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}


int round(float num) { 
    return num < 0 ? num - 0.5 : num + 0.5; 
} 

int abs(int num){
    return num < 0 ? -1 * num : num;
}

// generates a random number between min (inclusive) and max (exclusive)
int random_int(int seed, int min, int max){
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
    float root=square/3;
    int i;
    if (square <= 0) return 0;
    for (i=0; i<32; i++)
        root = (root + square / root) / 2;
    return root;
}

// cos estimation using taylor expansion
float cos(float deg){
    // convert to equivilant radian
    if(deg<0) deg = -1*deg;
    if(deg>360) deg -= ((int) (deg/360)) * 360; // mod 360
    float rad = deg * PI / 180;

    float res = 0;
    float term = 1;
    int k = 0;
    while (res + term != res){
        res += term;
        k += 2;
        term *= -1*rad*rad/k/(k-1);
    }
    return res;
}

// sin estimation using taylor expansion
float sin(float deg){
    // convert to equilivant radian
    float sign = 1;
    if(deg<0){
        sign = -1;
        deg = -1 * deg;
    }
    if(deg>360) deg -= ((int) deg/360) * 360; // mod 360
    float rad = deg * PI / 180;

    float res = 0;
    float term = rad;
    int k = 1;
    while(res+term != res){
        res += term;
        k += 2;
        term *= -1*rad*rad/k/(k-1);
    }
    return sign*res;
}

// draw pixels along line between 2 points
void draw_line(float xstart, float ystart, float xend, float yend){
    float slope = (yend - ystart) / (xend - xstart);
    int i;
    int yapprox, xapprox;
    float y; //holds current y value across iterations
    float x; //holds current x value across iterations
 
    // draw start and end pixels
    scene[round(xstart)][round(ystart)] = 1;
    scene[round(xend)][round(yend)] = 1;

    if(abs(xend-xstart) > abs(yend-ystart)){ // more lines between x
        if(xstart < xend){ // start at xstart and move forward
            y = ystart;
            for(i = xstart+1; i < xend; i++){
                y += slope;
                yapprox = round(y);
                scene[i][yapprox] = 1;
            }
        }
        else{ // start at xend and move backwards
            y = yend;
            for(i = xend+1; i < xstart; i++){
                y += slope;
                yapprox = round(y);
                scene[i][yapprox] = 1; 
            }
        }
    }
    else{ // more lines between y
        if(ystart < yend){ // start at ystart and move forwards
            x = xstart;
            for(i = ystart+1; i < yend; i++){
                x += 1 / slope;
                xapprox = round(x);
                scene[xapprox][i] = 1;
            }
        }
        else{ // start at yend and move backwards
            x = xend;
            for(i = yend+1; i < ystart; i++){
                x += 1 / slope;
                xapprox = round(x);
                scene[xapprox][i] = 1;
            }
        }
   }
}

// Draws a shape defined as a list of points 
// (draws a line from each point to the next in the array and finally from the last point to the first point)
void draw_shape(int size, float pointarr[]){
    int i;
    for(i=0; i<(size*2)-2; i = i+2){ // draw a line from each point to the next
        draw_line(pointarr[i], pointarr[i+1], pointarr[i+2], pointarr[i+3]);
    }
    draw_line(pointarr[size*2-2], pointarr[size*2-1], pointarr[0], pointarr[1]); // draw line from last point to first
}

int get_center_x(int size, float pointarr[])
{
  int i;
  float center_x = 0;
  for (i = 0; i <= (size * 2) - 2; i = i + 2)
  { // draw a line from each point to the next
    center_x += pointarr[i];
  }
  return center_x / size;
}

int get_center_y(int size, float pointarr[])
{
  int i;
  float center_y = 0;
  for (i = 0; i <= (size * 2) - 2; i = i + 2)
  { // draw a line from each point to the next
    center_y += pointarr[i + 1];
  }
  return center_y / size;
}


void rotate(float angle, float xcenter, float ycenter, int size, float* pointarr){
    int i;
    float xcentered;
    float xnew;
    float ynew;
    float c = cos(angle);
    float s = sin(angle);

    for(i=0; i < size; i++){
        xnew = *pointarr - xcenter;
        xcentered = xnew;
        pointarr += 1;
        ynew = *pointarr - ycenter;
        //pointarr -= 1;

        xnew = (c*xnew) - (s*ynew);
        ynew = (s*xcentered) + (c*ynew);

        
        printf("(%f, %f)\n", xnew + xcenter, ynew + ycenter);
        *pointarr = ynew + ycenter;
        pointarr -=1;
        *pointarr = xnew + xcenter;

        pointarr += 2; // point to next coord's x
    }
}

__uint8_t line_circle_collision(float xstart, float ystart, float xend, float yend, float xcircle, float ycircle, float rcircle){
    __uint8_t projOnLine; // holds bool after proj is calculated
    
    // project onto line
    float xline = xend - xstart;
    float yline = yend - ystart;
    float scale = ((xcircle - xstart) * xline) + ((ycircle - ystart) * yline);
    scale /= (xline*xline) + (yline*yline);
    float xD = (scale * xline) + xstart;
    float yD = (scale * yline) + ystart;
    //printf("projection: (%f,%f)\n", xD, yD);

    float projdist = sqroot(((xcircle - xD)*(xcircle - xD)) + ((ycircle - yD)*(ycircle - yD)));
    float startdist = sqroot(((xcircle - xstart)*(xcircle - xstart)) + ((ycircle - ystart)*(ycircle - ystart)));
    float enddist = sqroot(((xcircle - xend)*(xcircle - xend)) + ((ycircle - yend)*(ycircle - yend)));
    //printf("projdist: %f\n", projdist);
    //printf("startdist: %f\n", startdist);
    //printf("enddist: %f\n", enddist);

   
    if(xstart < xend){
        projOnLine = (xD >= xstart) & (xD <= xend);
    }
    else{
        projOnLine = (xD <= xstart) & (xD >= xend);  
    }

    // if end points are in circle
    if(startdist <= rcircle || enddist <= rcircle){
        return 1;
    }
    // if proj is in circle and in line segment
    if((projdist <= rcircle) & projOnLine){
        return 1;
    }
    else{
        return 0;
    }
}


int main(void){
    
    // set all pixels of screen to 0
    int i; int j;
    for(i=0; i<32; i++){
        for(j=0; j<16; j++){
            scene[i][j] = 0;
        }
    }

    // create and draw a square
    float square[] = {10, 3, 8, 13, 12, 13};
    draw_shape(3, square);
    print_game(scene);
    printf("\n\n");



    // erase screen
    for(i=0; i<32; i++){
        for(j=0; j<16; j++){
            scene[i][j] = 0;
        }
    }

    rotate(-5, get_center_x(3, square), get_center_y(3, square), 3, square);
    draw_shape(3, square);
    print_game(scene);

    printf("collision: %u\n", line_circle_collision(-2,1, 2,0, 0,0, 1));
    return 0;
}

