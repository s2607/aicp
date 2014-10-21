extern char *_estuff;
void   __attribute__ ((used)) start (char *a)
{
	*a=42;
	*(&_estuff) =0x10;//because gcc linker keeps ignoring the used attribute
}

