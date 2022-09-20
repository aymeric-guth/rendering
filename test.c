#include <string.h>
#include <string.h>
#include <stdio.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"



static const char *color_map[] = {
    [COLOR_BLACK] = "test10101hahahahihi",
    [COLOR_TEMPLATE] = "\x1b[38;5;$%dm"
};

void test()
{
    int x = 0;
    printf("pre=%d\n", ++x);
    x = 0;
    printf("post=%d\n", x++);
}

void fork1()
{
    char buff[100];
    char *c = buff;
    memset(c, 0, 100);
    strcpy(c, "\x1b[31;1m");
    c += 7;
    strcpy(c, "@");
    c++;
    strcpy(c, "\x1b[33;1m");
    c += 7;
    strcpy(c, "a");
    c++;
    strcpy(c, "\x1b[0m");
    c += 4;
    printf("%s\n", buff);
    int i = 0;

    while (i < 100) {
        printf("elmt %d = %x\n", i, buff[i]);
        i++;
    }
}

int main()
{
    // tty raw mode, non buffered io
    struct termios mode;
    tcgetattr(0, &mode);
    mode.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &mode);
    //
    char buff[100];
    memset(buff, 0, 100);
    unsigned long buffp = 0;
    const char *startcolor = "\x1b[31;1m";
    const char *val = "@";
    const char *endcolor = " \x1b[0m";
    strcpy(buff, startcolor);
    buffp += strlen(startcolor);
    strcpy(buff + buffp, val);
    buffp += strlen(val);
    strcpy(buff + buffp, endcolor);
    buffp += strlen(endcolor);
    printf("%s\n", buff);
    test();
    fork1();
    test();
}

//#include <ncurses.h>
//int main()
//{
//    // https://stackoverflow.com/questions/11301883/using-ncurses-to-capture-mouse-clicks-on-a-console-application
//    // keypad(window, TRUE);
//    while (1) {
//        int c = getch();
//        MEVENT event;
//
//        switch (c) {
//        case KEY_MOUSE:
//            if (getmouse(&event) == OK) {
//                if (event.bstate & BUTTON1_PRESSED)   // This works for left-click
//                    printf("BUTTON1_PRESSED\n");
//                else if (event.bstate & BUTTON2_PRESSED)   // This doesn't capture right-click
//                    printf("BUTTON1_PRESSED\n");
//                else
//                    fprintf(stderr, "Event: %lu", event.bstate); // Doesn't print anything on right-click
//            }
//
//            break;
//        }
//    }
//}
