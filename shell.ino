const int ledPin = 13;
extern char _umem ;
char buf[300];
unsigned int umeml=48000;
unsigned int chexad=0x0;
unsigned int ev;
int cr=0;
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
void write(void)
{
	char *c=&buf[1];
	unsigned int t;
	unsigned int l=(unsigned int)strtol(c,&c,16);
	char *m=(char *)l;
	Serial.println((unsigned int)m, HEX);
	if((l< (unsigned int)&_umem) || l>(unsigned int)&_umem+umeml)
	{
		Serial.println("no");
		return;
	}
	while((*c!=0) && (*c!='\n'))
	{
		Serial.print(" ");
		t=(unsigned int)strtol(c,&c,16);	
		*m=t;
		Serial.println((unsigned char)*m,HEX);
		m++;
	
	}
}
void ver(void)
{
	Serial.print("user mem begins at ");
	Serial.println((unsigned int)&_umem,HEX);
}
void read(void)
{
	char *c=&buf[1];
	unsigned int l=(unsigned int)strtol(c,&c,16);
	unsigned int le=(unsigned int)strtol(c,&c,16);

	Serial.println((unsigned int)(char *)l, HEX);
	for(;le>0;le--)
		Serial.print((unsigned int)*((char *)(l++)),HEX);
	Serial.println("--");

}
void ex()
{
	char *c=&buf[1];
	char ret=0;
	//unsigned int l=(unsigned int)strtol(c,&c,16);
	void (*m)(char*);
	m=(void (*)(char*))(void*)ev;
	Serial.print("Jumping to:");
	Serial.println((unsigned int)m,HEX);
	(*m)(&ret);
	Serial.print("Control returned succsesfully return value: ");
	Serial.println((unsigned int)ret,HEX);
	
	
}
int hni(int l, char **i,char *c)
{
	char n[9];
	int t=l;
	for(int a=0;l>0;l--){
		a=*(n+(t-l))=*((*i)+(t-l));
		*c=(*c)+(a<='9'?a-'0':a-'A');
	}
	*i=(*i)+t;
	n[t]=0;
	return strtol(n,NULL,16);
}
int hwrite(int bytes, unsigned int lad, char **i,char *c)
{
	unsigned int l=lad+chexad;
	if((l< (unsigned int)&_umem) || l>(unsigned int)&_umem+umeml)
	{
		Serial.println("no");
		return -1;
	}
	for(int j=0;j<=bytes;j++)
	{
		*(char *)(l+j)=(char)hni(2,i,c);
		Serial.print(*(char *)(l+j),HEX);
	}
	return 0;
}

void hex()
{

	char *i=&buf[1];
	char c;
	int bytes=hni(2,&i,&c);
	unsigned int lad=hni(4,&i,&c);
	int rtype=hni(2,&i,&c);
	
	Serial.println("got");
	Serial.println(bytes,HEX);
	Serial.println(lad,HEX);
	Serial.println(rtype,HEX);
	switch(rtype){
		case 0: hwrite(bytes,lad,&i,&c); break;
		case 1: Serial.println("Done");cr=0; break;
		case 4:
			Serial.print("new block: ");
			chexad=hni(4,&i,&c);
			chexad=chexad<<16;
			Serial.println(chexad,HEX);
			break;
		case 5: Serial.print("exec vector: ");
			ev=hni(8,&i,&c);
			Serial.println(ev,HEX);
			break;
		default: Serial.println("Oops");break;
	}
	hni(2,&i,&c);
	Serial.print("HEX:");
	if(c)
		Serial.print(" NUUH: ");
	else
		Serial.print(" YEEE: ");
	Serial.println(cr++,HEX);
}

void loop() {
	Serial.print("@");
	Serial.readBytesUntil('\n', buf, 162);
	switch(buf[0]){
		case 'w': write();break;
		case 'v': ver();break;
		case 'r': read();break;
		case 'e': ex();break;
		case ':': hex();break;
		default: Serial.println("?");
	}
}

