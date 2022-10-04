int strncmp(const char *s1, const char *s2, int n)
{
	register unsigned char u1, u2;

	while (n-- > 0) {
		u1 = (unsigned char)*s1++;
		u2 = (unsigned char)*s2++;
		if (u1 != u2)
			return u1 - u2;
		if (u1 == '\0')
			return 0;
	}
	return 0;
}

char *strchr(const char *s, int c)
{
	do {
		if (*s == c) {
			return (char *)s;
		}
	} while (*s++);
	return (0);
}

// TODO: need optimized
void memset ( void * ptr, int value, unsigned int sz )
{
	for(unsigned int i=0; i<sz; i++) {
		*((char*)ptr++) = value;
	}
}