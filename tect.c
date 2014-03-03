#include <stdio.h>
 
main()
{
    int dollars = 50;
    int tip = 15;
    int cents = 39;
    
    float dc = (float)dollars + (float)cents / 100;
    char buff[13];
    snprintf(buff, sizeof(buff),"amount %f ",dc);
    printf("%s\n", buff);
    return 0;
}
