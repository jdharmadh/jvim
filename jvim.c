#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#define RESETCOLOR "\033[0m"

struct termios orig_termios;
void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  setvbuf(stdout, NULL, _IONBF, 0);
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main(int argc, char **argv){
    enableRawMode();
    printf("\e[1;1H\e[2J");
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        printf("%c", c);
    }

    return 0;
}
