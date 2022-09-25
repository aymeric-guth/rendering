#include "loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int loader_obj(FILE *fp, Mesh **m)
{
    fseek(fp, 0, SEEK_END);
    const long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char buff[fsize];
    fread(buff, fsize, 1, fp);
    int bp = 0;

    while (bp < fsize) {
        int idx = bp;

        while (strcmp(&buff[bp], "\n"))
            bp++;

        if (strcmp(&buff[idx], "v")) {
        } else if (strcmp(&buff[idx], "f")) {
        } else {
        }
    }

    return 1;
}
