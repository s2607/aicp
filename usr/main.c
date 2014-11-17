extern char *_estuff;
void   __attribute__ ((used)) start (int (*entry)(int, char *))
{
	(*entry)(1,"hello world from userspace\n");
	int pin=2;
	int a;
		a=(6762/((*entry)(3,&pin)-9))-4;
		(*entry)(4,&a);
	*(&_estuff) =0x10;//because gcc linker keeps ignoring the used attribute
}

