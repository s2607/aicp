extern char *_estuff;
void   __attribute__ ((used)) start (int (*entry)(int, char *))
{
	(*entry)(1,"hello world from userspace\n");
	int pin=2;
	int a;
		a=(*entry)(3,&pin);
		(*entry)(4,&a);
	*(&_estuff) =0x10;//because gcc linker keeps ignoring the used attribute
}

