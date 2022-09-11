#include <stdio.h>

#define SCREEN_WIDTH 124

int main()
{
    for (int i = 0; i < SCREEN_WIDTH; i++)
        printf("%c", 126);

    printf("\n");
    return 0;
}
