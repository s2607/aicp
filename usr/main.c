extern char *_estuff;
void   __attribute__ ((used)) start (int (*entry)(int, char *))
{
	(*entry)(1,"hello world from userspace\n");
	
	*(&_estuff) =0x10;//because gcc linker keeps ignoring the used attribute
}

