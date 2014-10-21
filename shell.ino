#define MAX_SYMS 255
#define SCR_SIZE 32
const int ledPin = 13;
extern char _umem ;
char buf[300];
char scratch[SCR_SIZE];
unsigned int umeml=48000;
unsigned int chexad=0x0;
unsigned int ev;
int cr=0;
void *syms[MAX_SYMS];
int cursym=0;
// the setup() method runs once, when the sketch starts
int out(char *a)
{
	Serial1.print(a);
	return 0;
}
int motor(char *a)
{
	Serial2.print(a);
	return 0;
}
int adsym( void **a)
{
	char o[9];
	if(cr>MAX_SYMS)
		return -1;
	out("sym added ");
	out(itoa(cr,o,16));
	out((char *)*(a));
	syms[cr]=(*(a++));
	return cr;
}
int madsym(char *name, void *ptr)
{
	void *s []= {name,ptr};
	return adsym(&s[0]);

}
void defaultsyms(void)
{
	madsym("adsym",(void *)adsym);
	madsym("out",(void *)out);
	madsym("motor",(void *)motor);
	
}
void setup() {
	// initialize the digital pin as an output.
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	Serial1.begin(9600);
	Serial2.begin(115200);
 	Serial1.setTimeout(1000*100000);
	defaultsyms();
	Serial1.print("user mem begins at ");
	Serial1.println((unsigned int)&_umem,HEX);

}
void write(void)
{
	char *c=&buf[1];
	unsigned int t;
	unsigned int l=(unsigned int)strtol(c,&c,16);
	char *m=(char *)l;
	Serial1.println((unsigned int)m, HEX);
	if((l< (unsigned int)&_umem) || l>(unsigned int)&_umem+umeml)
	{
		Serial1.println("no");
		return;
	}
	while((*c!=0) && (*c!='\n'))
	{
		Serial1.print(" ");
		t=(unsigned int)strtol(c,&c,16);	
		*m=t;
		Serial1.println((unsigned char)*m,HEX);
		m++;
	
	}
}
void ver(void)
{
	Serial1.print("user mem begins at ");
	Serial1.println((unsigned int)&_umem,HEX);
}
void read(void)
{
	char *c=&buf[1];
	unsigned int l=(unsigned int)strtol(c,&c,16);
	unsigned int le=(unsigned int)strtol(c,&c,16);

	Serial1.println((unsigned int)(char *)l, HEX);
	for(;le>0;le--)
		Serial1.print((unsigned int)*((char *)(l++)),HEX);
	Serial1.println("--");

}
void ex()
{
	char *c=&buf[1];
	char ret=0;
	//unsigned int l=(unsigned int)strtol(c,&c,16);
	void (*m)(char*);
	m=(void (*)(char*))(void*)ev;
	Serial1.print("Jumping to:");
	Serial1.println((unsigned int)m,HEX);
	(*m)(&ret);
	Serial1.print("Control returned succsesfully return value: ");
	Serial1.println((unsigned int)ret,HEX);
	
	
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
		Serial1.println("no");
		return -1;
	}
	for(int j=0;j<=bytes;j++)
	{
		*(char *)(l+j)=(char)hni(2,i,c);
		Serial1.print(*(char *)(l+j),HEX);
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
	
	Serial1.println("got");
	Serial1.println(bytes,HEX);
	Serial1.println(lad,HEX);
	Serial1.println(rtype,HEX);
	switch(rtype){
		case 0: hwrite(bytes,lad,&i,&c); break;
		case 1: Serial1.println("Done");cr=0; break;
		case 4:
			Serial1.print("new block: ");
			chexad=hni(4,&i,&c);
			chexad=chexad<<16;
			Serial1.println(chexad,HEX);
			break;
		case 5: Serial1.print("exec vector: ");
			ev=hni(8,&i,&c);
			Serial1.println(ev,HEX);
			break;
		default: Serial1.println("Oops");break;
	}
	hni(2,&i,&c);
	Serial1.print("HEX:");
	if(c)
		Serial1.print(" NUUH: ");
	else
		Serial1.print(" YEEE: ");
	Serial1.println(cr++,HEX);
}
void esyms(void)
{
	char *c=&buf[1];
	int ret=0;
	int (*m)(char*);
	m=(int (*)(char*)) syms[strtol(c,&c,16)];
	Serial1.print("Jumping to:");
	Serial1.println((unsigned int)m,HEX);
	ret = (*m)(c);
	Serial1.print("Control returned succsesfully return value: ");
	Serial1.println((unsigned int)ret,HEX);


}
void loop() {
	Serial1.print("@");
	Serial1.readBytesUntil('\n', buf, 162);
	switch(buf[0]){
		case 'w': write();break;
		case 'v': ver();break;
		case 'r': read();break;
		case 'i': ex();break;
		case 'e': esyms();break;
		case ':': hex();break;
		default: Serial1.println("?");
	}
}

