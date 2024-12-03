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
    setup();
    Editor_Print(config);
    char c;
    while (config->running && read(STDIN_FILENO, &c, 1) == 1){
      // some asserts for debugging
      assert(config->running);
      assert(config->cursor.x >= 0);
      assert(config->cursor.y >= 1);
      assert(config->file->num_lines > 0);
      assert(config->cursor.y <= config->file->num_lines + 1);
      assert(config->cursor.x <= config->file->lines[config->cursor.y - 1]->line_length);
      Editor_ProcessKey(config, c);
      Editor_Print(config);
    }
    finish();
    return 0;
}

void setup() { 
  // setup editor config
  config = Editor_Setup();
  // enable raw mode
  setvbuf(stdout, NULL, _IONBF, 0);
  tcgetattr(STDIN_FILENO, &(config->orig_termios));
  struct termios raw = config->orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void finish() {
  printf("\e[1;1H\e[2J");
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &(config->orig_termios));
}
