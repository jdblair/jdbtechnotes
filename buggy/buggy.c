#include <stdio.h>
#include <string.h>
 
void copy_and_print(char *dest, char *src)
{
    strcpy(dest, src);
    printf("%s", src);
}
 
int main(int argc, char **argv)
{
    char *string = NULL;
 
    copy_and_print(string, "hello world!\n");
 
    return 0;
}
