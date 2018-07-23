#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>


static inline unsigned long 
process_time_ms()
{
    struct rusage ru;
    unsigned long usr_time;
    unsigned long sys_time;
    
    getrusage(RUSAGE_SELF,&ru);

    usr_time = ru.ru_utime.tv_sec * 1000 + ru.ru_utime.tv_usec/1000;
    sys_time = ru.ru_stime.tv_sec * 1000 + ru.ru_stime.tv_usec/1000;

    return usr_time + sys_time;
}

static inline unsigned long 
current_ms()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

static inline 
int cpusage(unsigned long clock_start, unsigned long clock_end, unsigned process_start, unsigned process_end)
{
    unsigned long process = process_end - process_start;
    unsigned long clock   = clock_end   - clock_start;
    
    if(process >= clock) {
        return 100;    
    }
    
    if(clock == 0) {
        return 0;    
    }

    return (process * 100)/clock;    
}



int main()
{
    int i,j,w;
    int n,m,usage;
    float a,b,c;
    unsigned long c0,c1,p0,p1;


    c0 = current_ms();
    p0 = process_time_ms();

    w = 100000;
    n = 60;
    m = 10000000;
    for(i = 0; i < n; i++){

        for(j = 0; j < m; j++) {
            a  += b * c;
            b +=  a * c;
            c +=  b * a;
        } 
        usleep(w);
        c1 = current_ms();
        p1 = process_time_ms();

        usage = cpusage(c0, c1, p0, p1);
        printf("%d\n",usage);
        c0 = c1;
        p0 = p1;


    }    
    

    return (int)a;
}

