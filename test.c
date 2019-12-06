#include <stdio.h>

const float PI = 3.1415926535;

struct Point{
    int x;
    int y;
}points[2];

__uint8_t scene[16][8];

// prints scene model
void print_game(){
    int x, y;
    for(x=0; x<8; x++){
        for(y=0; y<16; y++){
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


int main(void){
    
    // set all pixels of screen to 0
    int i; int j;
    for(i=0; i<16; i++){
        for(j=0; j<8; j++){
            scene[i][j] = 0;
        }
    }

    // create and draw a square
    float square[] = {0,0, 5,0, 5,5, 0,5};
    draw_shape(4, square);
    print_game(scene);
    printf("\n\n");



    // erase screen
    for(i=0; i<16; i++){
        for(j=0; j<8; j++){
            scene[i][j] = 0;
        }
    }

    rotate(45, 2.5, 2.5, 4, &square);
    draw_shape(4, square);
    print_game(scene);

    return 0;
}

