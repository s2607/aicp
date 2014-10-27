#define MAX_SYMS 255
#define SCR_SIZE 32

const int ledPin = 13;
extern char _umem ;
char buf[300];
char scratch[SCR_SIZE];

unsigned int umeml=48000;
unsigned int chexad=0x0;
unsigned int ev;

int (*syms[MAX_SYMS])(char*);
int cursym=0;
int out(char *a)
{
	Serial1.print(a);
	return 0;
}
int motor(char *a)
{
		int d,s;
	if(*(a)!=5)
	{
		d= *(a)>5 ? 1 :0;
		s= *(a) +(d ? 0 : 5);
		Serial3.print(d ?"1f":"1r");
		Serial3.print(s);
		Serial3.print("\r");
	}
	else
		Serial3.print("1f0\r");
	delay(10);

/*	while(Serial3.available())
	{
		Serial3.readBytesUntil('\r', scratch, SCR_SIZE);
		out(&scratch[0]);
	}*/
	out("motor 2\n");
	if(*(a+1)!=5)
	{
		d= *(a+1)>6 ? 1 :0;
		s= *(a+1) +(d ? 0 : 5);
		Serial3.print(d ?"2f":"2r");
		Serial3.print(s);
		Serial3.print("\r");
	}
	else
		Serial3.print("2f0\r");
		delay(10);
	while(Serial3.available())
	{
		Serial3.readBytesUntil('\r', scratch, SCR_SIZE);
		out(&scratch[0]);
	}
	for(int i=*(a+2);i>0;i--)
		delay(10);
	while(Serial3.available())
	{
		Serial3.readBytesUntil('\r', scratch, SCR_SIZE);
		out(&scratch[0]);
	}

	return 0;
}
void mmotor(void)
{
	char *c=&buf[1];
	char o[9];
	char a[]={(char)strtol(c,&c,16),(char)strtol(c,&c,16),(char)strtol(c,&c,16)};
	out(itoa(a[0],o,16));
	out(itoa(a[1],o,16));
	out(itoa(a[2],o,16));
	motor(&a[0]);
	a[0]=5;
	a[1]=5;
	a[2]=0;
	motor(&a[0]);
	motor(&a[0]);
	motor(&a[0]);
	motor(&a[0]); //SAFTY FIRST !!!!! :D:D:D:D

}

int adsym( void **a)
{
	char o[9];
	if(cursym>MAX_SYMS)
		return -1;
	out("sym added ");
	out(itoa(cursym,o,16));
	out((char *)*(a));
	syms[cursym]=((int (*)(char*))*(a+1));
	out(itoa((unsigned int)syms[cursym],o,16));
	out("\n");
	cursym=cursym+1;
	return cursym-1;
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
void init()
{
	int cursym=0;
	// initialize the digital pin as an output.
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	Serial1.begin(9600);
	Serial3.begin(115200);
 	Serial1.setTimeout(1000*100000);
	defaultsyms();
	Serial1.print("user mem begins at ");
	Serial1.println((unsigned int)&_umem,HEX);
}
void setup() {
init();
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
	init();
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
	static int cr=0;
	
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
	int s =(int)strtol(c,&c,16);
	Serial1.print((unsigned int)s,HEX);
	m=(int (*)(char*)) syms[s];
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
		case 'm': mmotor();break;
		case ':': hex();break;
		default: Serial1.println("?");
	}
}

