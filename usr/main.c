extern char *_estuff;
void   __attribute__ ((naked, used)) start (char *a)
{
	int b=0x100;
	*(&_estuff) =0x10;//because gcc linker keeps ignoring the used attribute
}

