/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

// RFID MFRC522 pins
// #define rf_ss	PORTE bit 1
// #define rf_sck	PORTE bit 2
// #define rf_mosi 	PORTE bit 3
// #define rf_miso 	PORTE bit 4
// #define rf_rst  	PORTE bit 5

//// A custom function to send data to SPI bus
unsigned char SPI_WB( unsigned char d ) {
  quicksleep( 50000 );
  unsigned char r = 0; // to get a MISO values
  unsigned char i;
  for(i=0; i < 8; i++){
  // rf_mosi = ( d >> i ) & 0x01;
  if (( d >> (7-i) ) & 0x01){
	  PORTESET = 0x8;
  }else{
	  PORTECLR = 0x8;
  }
  quicksleep( 10000 );
  PORTESET = 0x4;		// rf_sck = 1;
  quicksleep( 10000 );
  r = (((PORTE & 0x1) << (7-i)) | r);    // r = ( ( rf_miso << i ) | r );
  quicksleep( 10000 );
  PORTECLR = 0x4;		// rf_sck = 0;
  quicksleep( 100000 );
  }
  quicksleep( 10000 );
  return r;
}

// A custom function to write data to register
void SPI_WR( unsigned char a, unsigned char v ) {	//a = address, v = 
  PORTECLR = 0x1;	//rf_ss = 0;
  SPI_WB( ( a << 1 ) & 0x7E ); // Address format: 0XXXXXX0
  SPI_WB( v );
  PORTESET = 0x1;	//rf_ss = 1;
}

// A custom function to read data from a register
unsigned char SPI_RR( unsigned char a ) {
  PORTECLR = 0x1; //rf_ss = 0;
  SPI_WB( ( ( a << 1 ) & 0x7E ) | 0x80 );
  PORTESET = 0x1;	//rf_ss = 1;
  return SPI_WB( 0x00 );
}

#define MFRC522_CR 0x01 // Command register
#define MFRC522_SR 0x0F // MFRC522 Soft Reset command
#define MFRC522_V 0x37 // MFRC522 Version register


//void main( ) {
//  rf_ss = 1;
//  quicksleep( 1000 );
//  SPI_WR( MFRC522_CR, MFRC522_SR ); // A result of logic picture is below
//  unsigned char str;
//  sprintf( str, "Version: 0x%x", SPI_RR( MFRC522_V ) );// Unfortunately returns "Version: 0x0"
//  while( 1 ) { }
//}

int main(void) {
        /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;												//NOTE THIS RESET
	LATB = 0x0;													//NOTE THIS CLEAR
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
	
	display_init();
	//display_string(0, "KTH/ICT lab");
	//display_string(1, "in Computer");
	//display_string(2, "Engineering");
	//display_string(3, "Welcome!");
	display_update();

	labinit();

	quicksleep( 100000 );
	SPI_WR( MFRC522_CR, MFRC522_SR ); // A result of logic picture is below
	unsigned char str;
	LATB = SPI_RR( MFRC522_V );// Unfortunately returns "Version: 0x0"
	while( 1 ){
		labwork();
	}
	return 0;
}
