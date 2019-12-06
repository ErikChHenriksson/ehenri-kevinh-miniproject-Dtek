#include <stdio.h>

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

// draw pixels along line between 2 points
void draw_line(int xstart, int ystart, int xend, int yend){
    float slope = (float)(yend - ystart) / (float)(xend - xstart);
    int i;
    int yapprox, xapprox;
    float y; //holds current y value across iterations
    float x; //holds current x value across iterations
 
    // draw start and end pixels
    scene[xstart][ystart] = 1;
    scene[xend][yend] = 1;

    if(abs(xend-xstart) > abs(yend-ystart)){ // more lines between x
        if(xstart < xend){ // start at xstart and move forward
            y = (float) ystart;
            for(i = xstart+1; i < xend; i++){
                y += slope;
                yapprox = round(y);
                scene[i][yapprox] = 1;
            }
        }
        else{ // start at xend and move backwards
            y = (float) yend;
            for(i = xend+1; i < xstart; i++){
                y += slope;
                yapprox = round(y);
                scene[i][yapprox] = 1; 
            }
        }
    }
    else{ // more lines between y
        if(ystart < yend){ // start at ystart and move forwards
            x = (float) xstart;
            for(i = ystart+1; i < yend; i++){
                x += 1 / slope;
                xapprox = round(x);
                scene[xapprox][i] = 1;
            }
        }
        else{ // start at yend and move backwards
            x = (float) xend;
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
void draw_shape(int size, int pointarr[]){
    int i;
    for(i=0; i<(size*2)-2; i = i+2){ // draw a line from each point to the next
        printf("(%d,%d) to (%d,%d)\n", i, i+1, i+2, i+3);
        draw_line(pointarr[i], pointarr[i+1], pointarr[i+2], pointarr[i+3]);
    }
    draw_line(pointarr[size*2-2], pointarr[size*2-1], pointarr[0], pointarr[1]); // draw line from last point to first
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
    int square[] = {0,0, 5,0, 5,5, 0,5};
    draw_shape(4, square);
    print_game(scene);

    int y;
    for(y=1; y<10; y++){
        printf("%d\n",random_int(i, 0, 2));
    }

    return 0;
}

