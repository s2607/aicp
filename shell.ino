const int ledPin = 13;
extern char _umem ;
char buf[162];
unsigned int umeml=48000;

// the setup() method runs once, when the sketch starts

void setup() {
	// initialize the digital pin as an output.
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	Serial.begin(9600);
 	Serial.setTimeout(1000*100000);
	Serial.print("user mem begins at ");
	Serial.println((unsigned int)&_umem,HEX);

}

// the loop() methor runs over and over again,
// as long as the board has power
unsigned int nexti( char **c)
{
	char *b=*c;
	for(;((**c!=0)&&(**c!=' '));*c++);
	return (unsigned int)strtol(b,c,16);
}
void write(void)
{
	char *c=&buf[1];
	unsigned int l=nexti(&c);
	char *m=(char *)l;
	if((l< (unsigned int)&_umem) || l>(unsigned int)&_umem+umeml)
	{
		Serial.println("no");
		return;
	}
	while(*c!=0)
	{
		*m=nexti(&c);
		m++;
	}
}

void loop() {
	Serial.print("@");
	Serial.readBytesUntil('\n', buf, 162);
	switch(buf[0]){
		case 'w': write();break;
		default: Serial.println("?");
	}
}

