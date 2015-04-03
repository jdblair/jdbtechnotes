#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* constants used for bitmask wizardry below */
long mask1  = 0x5555555555555555;   // 01010101010101010101010101010101....
long mask2  = 0x3333333333333333;   // 00110011001100110011001100110011....
long mask4  = 0x0f0f0f0f0f0f0f0f;   // 00001111000011110000111100001111....
long mask8  = 0x00ff00ff00ff00ff;   // 00000000111111110000000011111111....
long mask16 = 0x0000ffff0000ffff;   // 00000000000000001111111111111111....
long mask32 = 0x00000000ffffffff;   // 00000000000000000000000000000000ffffff....
long h01    = 0x0101010101010101;   // 00010001000100010001000100010001....
                                       

int naive(long v)
{
    int bit = 0;
    int count = 0;
    for (bit = 0; bit < sizeof(v) * 8; bit++) {
        count += v & 0x01;
        v >>= 1;
    }
    
    return count;
}

int lut_4bit(long v)
{
    static int lut[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

    int count = 0;
    int nybble = 0;
    for (nybble = 0; nybble < sizeof(v) * 2; nybble++) {
        count += lut[v & 0x0f];
        v >>= 4;
    }

    return count;
}

int lut_8bit(long v)
{
    static int lut[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
                           1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
                           1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
                           2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                           1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
                           2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                           2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                           3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                           1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
                           2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                           2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                           3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                           2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
                           3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                           3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
                           4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};    

    int count = 0;
    int byte = 0;
    for (byte = 0; byte < sizeof(v) * 2; byte++) {
        count += lut[v & 0xff];
        v >>= 8;
    }
    
    return count;
}

int hacker(long v)
{
    v = (v & mask1) + ((v >> 1)  & mask1);
    v = (v & mask2) + ((v >> 2)  & mask2);
    v = (v & mask4) + ((v >> 4)  & mask4);
    v = (v & mask8) + ((v >> 8)  & mask8);
    v = (v & mask16) + ((v >> 16) & mask16);
    v = (v & mask32) + ((v >> 32) & mask32);

    return v;
}

int hacker2(long v)
{
    v = v - ((v >> 1) & mask1);
    v = (v & mask2) + ((v >> 2) & mask2);
    v = (v + (v >> 4)) & mask4;
    v = v + (v >> 8);
    v = v + (v >> 16);
    v = v + (v >> 32);

    return v & 0x7f;
}

int hacker3(long v)
{
    v = v - ((v >> 1) & mask1);
    v = (v & mask2) + ((v >> 2) & mask2);
    v = (v + (v >> 4)) & mask4;
    return (v * h01)>>56;
}



int control(long v)
{
    return 1;
}

int main(int argc, char **argv)
{
    int (*bitcount_f)(long v);

    if (argc < 2) {
        printf("please specify which test\n");
        return 1;
    }
    
    if (strcmp("naive", argv[1]) == 0) {
        bitcount_f = naive;
    } else if (strcmp("lut_4bit", argv[1]) == 0) {
        bitcount_f = lut_4bit;
    } else if (strcmp("lut_8bit", argv[1]) == 0) {
        bitcount_f = lut_8bit;
    } else if (strcmp("hacker", argv[1]) == 0) {
        bitcount_f = hacker;
    } else if (strcmp("hacker2", argv[1]) == 0) {
        bitcount_f = hacker2;
    } else if (strcmp("hacker3", argv[1]) == 0) {
        bitcount_f = hacker3;
    } else if (strcmp("control", argv[1]) == 0) {
        bitcount_f = control;
    }

    int i;
    unsigned int sum = 0;

    for (i = 0; i < 1000000000; i++) {
        sum += bitcount_f(rand());
    }

    /* print out sum so the compiler doesn't optimize it all away */
    printf("sum = %u\n", sum);

    return 0;
}
