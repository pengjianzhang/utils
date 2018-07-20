#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef     struct 
{
    uint64_t lo;
    uint64_t hi;
} __attribute__ (( __aligned__( 16 ) )) uint128_t;


#define cas128(addr, old, new) ( {\
    unsigned char ret;\
    __asm__ __volatile__\
    (\
    "lock cmpxchg16b  %1\n\t"\
    "setz %0"\
    : "=q" ( ret )\
    , "+m" ( *addr)\
    , "+d" ( old.hi )\
    , "+a" ( old.lo )\
    : "c" ( new.hi )\
    , "b" ( new.lo )\
    : "cc"\
    ); ret;})

void test(uint64_t a1, uint64_t a2, uint64_t b1, uint64_t b2, uint64_t c1, uint64_t c2)
{
    uint128_t data;
    uint128_t new; 
    uint128_t old; 
    int ret;

    data.lo = a1;
    data.hi = a2;

    old.lo = b1;
    old.hi = b2;

    new.lo = c1;
    new.hi = c2;

    ret = cas128( &data, old, new);

    printf("case ((%lu,%lu) , (%lu,%lu) , (%lu, %lu) => %d\n", a1,a2,b1,b2,c1,c2,ret);
}


int main()
{


    test(1,1,1,1,8,9);
    test(1,2,1,2,8,9);
    test(1,2,1,1,8,9);
    test(1,2,2,1,8,9);

    return 0;
} 
