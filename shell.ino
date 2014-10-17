//ROBOTS ARE BETTER THEN GIRLFREINDS
#define LED_N_SIDE 2
#define LED_P_SIDE 3
#define LAMP 13
#define LSF 9
#define LSR 115

#define RSF 114
#define RSR 9

#define LSP 10
#define RSP 9
#define BOUSTT_T 1000  //aproxomet time for a 100 gradian tank turn
#define BOUSTF_T 1000  //aproxomet time to travel width of robot mop
#define servopower 11

#include <Servo.h>
const int ledPin = 13;
extern char _umem ;

// the setup() method runs once, when the sketch starts

void setup() {
	// initialize the digital pin as an output.
	pinMode(ledPin, OUTPUT);
	Serial.begin(9600);
}

// the loop() methor runs over and over again,
// as long as the board has power

void loop() {
	digitalWrite(ledPin, HIGH);		// set the LED on
	delay(1000);									// wait for a second
	digitalWrite(ledPin, LOW);		// set the LED off
	delay(1000);									// wait for a second
	Serial.println("hello world");
	Serial.println((unsigned int)&_umem,HEX);
}

