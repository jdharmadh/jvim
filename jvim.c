#include <stdio.h>

int main(int argc, char **argv){
    printf("\e[1;1H\e[2J");
    printf("Hello World!\n");
    return 0;
}
