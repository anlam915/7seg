/*
 * First test module for 7seg display
 * Manually display (or terminal input) a digit on the display
 *
 * Use 8 GPIO pins and define the bit fields for each digit
 * Can i do this with an IC and use less gpio pins?
 */ 

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <stdint.h>
#include <stdbool.h>

#define NUM_SEG 8
#define NUM_BITS 4

#define A_SEG	38
#define B_SEG 	40
#define C_SEG 	31
#define D_SEG 	35
#define E_SEG 	33
#define F_SEG 	36
#define G_SEG 	32
#define DP_SEG 	37
#define INTR	15

#define BIT0	22
#define BIT1	18
#define BIT2	16
#define BIT3	12

#define ZERO	0b11111100
#define ONE 	0b01100000
#define TWO 	0b11011010
#define THREE	0b11110010
#define FOUR	0b01100110
#define FIVE	0b10110110
#define SIX	0b10111110
#define SEVEN	0b11100000
#define EIGHT	0b11111110
#define NINE	0b11100110

#define ON(y) 	digitalWrite((y), HIGH)
#define OFF(y)	digitalWrite((y), LOW)


//enum digit(ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE);
//enum seg(A_SEG = 38, B_SEG = 40, C_SEG = 31, D_SEG = 35, E_SEG = 33,
//	 F_SEG = 36, G_SEG = 32, DP_SEG = 37);

static const int SEG_PIN[] = {DP_SEG, G_SEG, F_SEG, E_SEG, D_SEG, C_SEG, B_SEG, A_SEG};
static const int BCD_PIN[] = {BIT0, BIT1, BIT2, BIT3};
static const uint8_t DIGIT[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};


void turnOff(void);

/* Displays the num argument on the display(must between 0 and 9) */
void displayNum(uint8_t);

/* Cycles  numbers 0 thru 9 on 7seg display */
void cycle(void);

/* Display number in BCD on LEDs */
void bcdLed(uint8_t);

/* ISR to increment eventCount */
void myISR(void);


volatile int eventCount = 0;
bool buttonPressed = FALSE;

int main(void){

	wiringPiSetupPhys();

	printf("hello %x \n",0xFFFFFFFF);
	
	// Init GPIO for 7seg
	pinMode(A_SEG, OUTPUT);
	pinMode(B_SEG, OUTPUT);
	pinMode(C_SEG, OUTPUT);
	pinMode(D_SEG, OUTPUT);
	pinMode(E_SEG, OUTPUT);
	pinMode(F_SEG, OUTPUT);
	pinMode(G_SEG, OUTPUT);
	pinMode(DP_SEG, OUTPUT);

	// Init GPIO for BCD LEDs bit 0 thru 3
	pinMode(BIT0, OUTPUT);
	pinMode(BIT1, OUTPUT);
	pinMode(BIT2, OUTPUT);
	pinMode(BIT3, OUTPUT);

	// Init Interrupt pin for push button
	pinMode(INTR, INPUT);

	pullUpDnControl(INTR, PUD_DOWN);

	if( wiringPiISR(INTR, INT_EDGE_FALLING, &myISR) < 0){
		fprintf(stderr, "Failed to setup ISR\n");
		return 1;
	}	
	


	turnOff();
	usleep(1000000);

	while(1){
		//cycle();
		displayNum(eventCount);
		bcdLed(eventCount);
		usleep(1000000);
	}

	return 0;
}



void turnOff(void){
	OFF(A_SEG);
	OFF(B_SEG);
	OFF(C_SEG);
	OFF(D_SEG);
	OFF(E_SEG);
	OFF(F_SEG);
	OFF(G_SEG);
	ON(DP_SEG);
}
void displayNum(uint8_t num){

	printf("Inside displayNum: %d\n", num);
	uint8_t my_digit = DIGIT[num]; // bit field for number to display
	
	for(int current_seg = 0; current_seg < NUM_SEG; current_seg++){
		if(my_digit & (1 << current_seg)){
			ON(SEG_PIN[current_seg]);
		}
		else{
			OFF(SEG_PIN[current_seg]);
		}

	}

}
void cycle(void){

	for(int count  = 0; count < 10; count++){		// for loop for DIGIT
		//printf("Lets display %d\n", count);
		displayNum(count);			
		usleep(500000);
		turnOff();
	}
	turnOff();
	usleep(500000);

}

void bcdLed(uint8_t num){
	printf("Inside bcdLed: %d\n", num);
	for(int current_bit = 0; current_bit < NUM_BITS; current_bit++){
		if(num & (1 << current_bit)){
			ON(BCD_PIN[current_bit]); // BCD_PIN[current_bit] resolves to the pin # for bit 'n'
		}
		else{
			OFF(BCD_PIN[current_bit]);
		}
	}
}
void myISR(void){
	eventCount = (eventCount + 1) % 10;
	usleep(800000);		// Debounce??
	printf("Button press detected, eventCount = %d\n", eventCount);

}

