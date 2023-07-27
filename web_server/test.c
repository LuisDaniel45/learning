#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *fp = fopen("/home/danyel/Music/nasa.mp3", "r");
    if (fp == NULL)
        return 1;

    FILE *fr = fopen("foo.mp3", "w");
    int n = 0;
    /** char str[4]; */
    while (1) 
    {
        uint8_t *byte;
        fread(byte, 1, sizeof(int), fp);
        if (*byte == 0x42)
            break;

        fwrite(byte, 1, sizeof(uint8_t), fr);
        printf(" %x", *byte);
        if (*byte < 0x10)
            printf(" ");
        n++;
        if (n == 4)
        {
            n = 0;
            printf(" ||  \n");
        }
    }
    fclose(fp);
    return 0; 
}
