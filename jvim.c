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

void setup(char *filename);
void finish(char *filename);

int main(int argc, char **argv){
    setup(argv[1]);
    Editor_Print(config);
    char c;
    while (config->running && read(STDIN_FILENO, &c, 1) == 1){
      Editor_ProcessKey(config, c);
      Editor_PrintCursor(config);
    }
    finish(argv[1]);
    return 0;
}

void setup(char *filename) { 
  //open the file
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening file\n");
    exit(1);
  }
  // setup the file
  config = Editor_FreshSetup();
  char c;
  while ((c = fgetc(file)) != EOF) {
    TextFile_AppendChar(config->file, c);
  }
  fclose(file);
  // get the window size
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &(config->window_size));
  // enable raw mode
  setvbuf(stdout, NULL, _IONBF, 0);
  tcgetattr(STDIN_FILENO, &(config->orig_termios));
  struct termios raw = config->orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void finish(char *filename) {
  // open the file
  FILE *file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "Error opening file\n");
    exit(1);
  }
  // write the file
  for (int i = 0; i < config->file->num_lines - 1; i++) {
    fprintf(file, "%.*s\n", config->file->lines[i]->line_length, config->file->lines[i]->text);
  }
  fprintf(file, "%.*s", config->file->lines[config->file->num_lines - 1]->line_length, config->file->lines[config->file->num_lines - 1]->text);
  fclose(file);
  // disable raw mode
  printf("\e[1;1H\e[2J");
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &(config->orig_termios));
  // free all memory
  Editor_Free(config);
}
