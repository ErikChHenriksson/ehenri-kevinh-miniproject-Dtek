#include <stdio.h>

void convert(__uint8_t game_state[128][32]){
    int i, j, k;
    int eight_bit_encoding;

    for(i=0; i<4; i++){ // for each screen quadrant
        for(j=0; j<128; j++){ // for each column

        
            // encode the column into 8-bits
            eight_bit_encoding = 0
            for(k=0; k<8; k++){
                //printf("%u ", game_state[j][8*i + k]);
                //printf("(%d, %d)", (j), (8*i + k));
                eight_bit_encoding |= 1 << k;  //game_state[j][8*i + k] << k;
                printf("  %u   ", eight_bit_encoding);
            }
            printf("     encoded: %u\n", eight_bit_encoding);
        }
    }
}


int main(void){
    int i; int j;
    __uint8_t scene[128][32];
    
   for(i=0; i<128; i++){
       for(j=0; j<32; j++){
           scene[i][j] = 1;
       }
   }


    convert(scene);

    //Xint z;
    //Xfor(z=0; z < 10; z++){
    //X    __uint8_t encode = 0;
    //X    int k;
    //X    for(k = 0; k<8; k++){
    //X        encode |= 1 << k;
    //X    }
    //X    printf("%u\n", encode);
    //X}

    return 0;
}
