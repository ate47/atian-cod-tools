#define AAA
#define BBB

#ifdef AAA
aaa
#else
not aaa
#endif


#ifdef AAA
#ifdef AAA
aaa
#else
not aaa
#endif
#else
not aaa
#ifndef BBB
#ifdef AAA
aaa
#endif
#endif
#endif