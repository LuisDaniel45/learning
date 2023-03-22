#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *fp = fopen("template.html", "r");
    if (fp == NULL)
        return 1;

    int len = 0;
    for (char c = fgetc(fp); c != EOF; c = fgetc(fp))
    {
        printf("%c", c);
        len++;
    }
    printf("%i\n", len);

    return 0;
}
