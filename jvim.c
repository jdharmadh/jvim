#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "layouts.h"
#include "text.h"
#include "colors.h"
#include "editor.h"

EditorConfig *config;

void setup();
void finish();

int main(int argc, char **argv){
    if (argc != 2) {
        fprintf(stderr, "Usage: jvim <filename>\n");
        exit(EXIT_FAILURE);
    }
    config = Editor_FreshSetup(argv[1]);
    setup();
    Editor_Print(config);
    char c;
    while (config->running && read(STDIN_FILENO, &c, 1) == 1){
      Editor_ProcessKey(config, c);
      Editor_PrintCursor(config);
    }
    finish();
    return EXIT_SUCCESS;
}

void setup() { 
  // get the window size
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &(config->window_size));
  // enable raw mode
  setvbuf(stdout, NULL, _IONBF, 0);
  tcgetattr(STDIN_FILENO, &(config->orig_termios));
  struct termios raw = config->orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void finish() {
  // disable raw mode
  printf("\e[1;1H\e[2J");
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &(config->orig_termios));
  // free all memory
  Editor_Free(config);
}
