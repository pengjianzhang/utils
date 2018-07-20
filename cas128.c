#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef     struct 
{
    uint64_t lo;
    uint64_t hi;
} __attribute__ (( __aligned__( 16 ) )) uint128_t;


inline unsigned char cas(uint128_t * addr, uint128_t old, uint128_t new)
{
    unsigned char ret;
    __asm__ __volatile__
    (
    "lock cmpxchg16b  %1\n\t"
    "setz %0"
    : "=q" ( ret )
    , "+m" ( *addr)
    , "+d" ( old.hi )
    , "+a" ( old.lo )
    : "c" ( new.hi )
    , "b" ( new.lo )
    : "cc"
    );
    return ret;
}

int main()
{
    uint128_t data =  { 1, 2 };
    uint128_t new  =  { 3, 4};
    uint128_t old  =  data;

    old.lo  = 2;
    int ret = cas( &data, old, new);

    printf("cas %d\n",ret);

    return 0;
} 
