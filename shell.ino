#define MAX_SYMS 255
#define SCR_SIZE 32
#define RES  2
// change HARDWARE AVG to 1, 2, 4, 8, 16, 32
#define AVG  0
#define S_C 1
#include <Servo.h>

const int ledPin = 13;
extern char _umem ;
char buf[300];
char scratch[SCR_SIZE];

unsigned int umeml=48000;
unsigned int chexad=0x0;
unsigned int ev=0;
int run=0;
int (*syms[MAX_SYMS])(char*);
int cursym=0;
int busywait(int);
void stop(void);
Servo servos[S_C];
int spos[S_C];
Serial cons;

//extern void snap(void);
int busywait(int a)
{
	for(int i=0;i<a/10;i++)
	{
		delay(15);
		servos[0].write(spos[0]);
	}
	return 0;
}
void nsnap(int *a)
{
	int tl=140; //slightly more then twice scan line length
	int l=40;
//	int b[l];
	int i=0;
//	Serial.begin(115200);
	analogReadResolution(RES);
	analogReadAveraging(AVG);
	analogReference(0);
   ADC0_CFG1 =  ADC_CFG1_ADIV(1) //
        | ADC_CFG1_MODE(2)  // 16 bit.
        | ADC_CFG1_ADICLK(0);  // bus clock
    ADC0_CFG2 = ADC_CFG2_MUXSEL; // select the "b" channels.
    ADC0_SC2 = ADC_SC2_REFSEL(0); // select the internal reference
    ADC0_SC3 =  0 ; // no averaging

ADC0_SC3 |= ADC_SC3_CAL; // begin cal

    while (ADC0_SC3 & ADC_SC3_CAL) {
    }
    // Record the calibration.  This is all taken directly from Pauls' analog.c
    uint16_t sum = ADC0_CLPS + ADC0_CLP4 + ADC0_CLP3 + ADC0_CLP2 + ADC0_CLP1 + ADC0_CLP0;
    sum = (sum / 2) | 0x8000;
    ADC0_PG = sum;
    sum = ADC0_CLMS + ADC0_CLM4 + ADC0_CLM3 + ADC0_CLM2 + ADC0_CLM1 + ADC0_CLM0;
    sum = (sum / 2) | 0x8000;
    ADC0_MG = sum;
	int st=micros();
	for(i=0;i<l;i++)
		*(a+i)=analogRead(0);
;
	cons.println(micros()-st);
	cons.println(i);
}
int out(char *a)
{
	cons.print(a);
	return 0;
}
int outn(char *a)
{
	char o[9];
	out(itoa(*((int *)a),o,16));
	out("\n");

}
int motor(char *a)
{
	char o[9];
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
	delay(30);

/*	while(Serial3.available())
	{
		Serial3.readBytesUntil('\r', scratch, SCR_SIZE);
		out(&scratch[0]);
	}*/
	out("motor 2\n");
	if(*(a+1)!=5)
	{
		d= *(a+1)>5 ? 1 :0;
		s= *(a+1) +(d ? 0 : 5);
		Serial3.print(d ?"2f":"2r");
		Serial3.print(s);
		Serial3.print("\r");
	}
	else
		Serial3.print("2f0\r");
		delay(10);
	/*while(Serial3.available())
	{
		Serial3.readBytesUntil('\r', scratch, SCR_SIZE);
		out(&scratch[0]);
	}*/
	for(int i=*(a+2);i>0;i--)
	{
		out(itoa(i,o,16));	
		delay(100);
	}
	out("tc\n");
	/*while(Serial3.available())
	{
		Serial3.readBytesUntil('\r', scratch, SCR_SIZE);
		out(&scratch[0]);
	}*/

	return 0;
}
int iservo(char *a)
{
	servos[*a].attach(3);
	return 0;	

}
int dservo(char *a)
{
	servos[(int)*a].detach();
}
int setservo (char *a)
{
	for(int i=0; i<10000; i++)
	spos[(int)*a]=*(a+1);
	return 0;
}
void msetservo (void)
{
	char *c=&buf[1];
	char o[9];
//	char a[]={(char)strtol(c,&c,16),(char)strtol(c,&c,16)};
//	out(itoa(a[0],o,16));
//	out(itoa(a[1],o,16));

//	setservo(&a[0]);
	spos[strtol(c,&c,16)]=strtol(c,&c,16);
/*	for(int i=0; i<1000; i++)
	{
		delay(10);
		servos[0].write(spos[0]);
	}*/

	out("set");
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
	out("retmmotor");
	a[0]=5;
	a[1]=5;
	a[2]=0;
	motor(&a[0]);
	out("off");

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
int aread(char *a)
{
	return analogRead((int)*a);
}
void maread(void)
{
	char o[9];
	char *c=&buf[1];
	char a=strtol(c,&c,16);
	out(itoa(aread(&a),o,16));
}
int ustop(char *a)
{
	stop();
}
int udelay(char *a)
{
	return busywait(*((int *)a));
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
	madsym("aread",(void *)aread);
	madsym("nout",(void *)outn);
	
}
void init()
{
	int cursym=0;
	cons = cons;
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, HIGH);
	cons.begin(9600);
	Serial3.begin(115200);
 	cons.setTimeout(1000*100000);
	defaultsyms();
	iservo(0);
	cons.print("user mem begins at ");
	cons.println((unsigned int)&_umem,HEX);
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
	cons.println((unsigned int)m, HEX);
	if((l< (unsigned int)&_umem) || l>(unsigned int)&_umem+umeml)
	{
		cons.println("no");
		return;
	}
	while((*c!=0) && (*c!='\n'))
	{
		cons.print(" ");
		t=(unsigned int)strtol(c,&c,16);	
		*m=t;
		cons.println((unsigned char)*m,HEX);
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

	cons.println((unsigned int)(char *)l, HEX);
	for(;le>0;le--)
		cons.print((unsigned int)*((char *)(l++)),HEX);
	cons.println("--");

}
int entry(int s, char *a)
{
	return (*syms[s])(a);	

}
void ex()
{
	char *c=&buf[1];
	char ret=0;
	//unsigned int l=(unsigned int)strtol(c,&c,16);
	void (*m)(int (*)(int, char *));
	m=(void (*)(int (*)(int, char *)))(void*)ev;
	cons.print("Jumping to:");
	cons.println((unsigned int)m,HEX);
	(*m)(entry);
	cons.print("Control returned succsesfully return value: ");
	cons.println((unsigned int)ret,HEX);
	run=1;
	
}
void stop()
{
	run=0;
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
		cons.println("no");
		return -1;
	}
	for(int j=0;j<=bytes;j++)
	{
		*(char *)(l+j)=(char)hni(2,i,c);
		cons.print(*(char *)(l+j),HEX);
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
	
	cons.println("got");
	cons.println(bytes,HEX);
	cons.println(lad,HEX);
	cons.println(rtype,HEX);
	switch(rtype){
		case 0: hwrite(bytes,lad,&i,&c); break;
		case 1: cons.println("Done");cr=0; break;
		case 4:
			cons.print("new block: ");
			chexad=hni(4,&i,&c);
			chexad=chexad<<16;
			cons.println(chexad,HEX);
			break;
		case 5: cons.print("exec vector: ");
			ev=hni(8,&i,&c);
			cons.println(ev,HEX);
			break;
		default: cons.println("Oops");break;
	}
	hni(2,&i,&c);
	cons.print("HEX:");
	if(c)
		cons.print(" NUUH: ");
	else
		cons.print(" YEEE: ");
	cons.println(cr++,HEX);
}
void esyms(void)
{
	char *c=&buf[1];
	int ret=0;
	int l;
	int (*m)(char*);
	int s =(int)strtol(c,&c,16);
	cons.print((unsigned int)s,HEX);
	c++;
	l=strlen(c);
	char a[l];
	hni((l/2)-1,&c,&a[0]);
	m=(int (*)(char*)) syms[s];
	cons.print("Jumping to:");
	cons.println((unsigned int)m,HEX);
	cons.print("ARGS");
	for(int i=0;i<=l/2;i++)
		cons.print((unsigned int)a[i],HEX);
	cons.println(" .");
	ret = (*m)(&a[0]);
	cons.print("Control returned succsesfully return value: ");
	cons.println((unsigned int)ret,HEX);


}
void loop() {
	int ibuf[200];
	if(cons.available())
	{
		cons.print("@");
		cons.readBytesUntil('\n', buf, 162);
		switch(buf[0]){
			case 'w': write();break;
			case 'v': ver();break;
			case 'r': read();break;
			case 'i': ex();break;
			case 'e': esyms();break;
			case 'm': mmotor();break;
			case 'a': maread();break;
			case 's': stop();break;
			case 'c': nsnap(&ibuf[0]);break;
			case 'o': msetservo();break;
			case ':': hex();break;
			default: cons.println("?");
		}
	}
	if(ev && run)
		ex();
	 busywait(500);
}

