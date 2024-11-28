#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#include "layouts.h"
#include "text.h"
#include "colors.h"

EditorConfig config;

void setup();
void finish();
void Editor_MoveCursor(int row_change, int col_change);
void Editor_PrintCursor();
void Editor_SetCursor(TextPos pos);

int main(int argc, char **argv){
    setup();
    // setup
    TextFile* file = TextFile_Allocate();
    config.file = file;
    TextFile_Print(file);
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1){
      if(c == 127 || c == 8){
        Editor_SetCursor(TextFile_DeleteChar(file, config.cursor));
        Editor_MoveCursor(0, -1);
      } else if (c == 'w'){
        Editor_MoveCursor(-1, 0);
      } else if (c == 'a'){
        Editor_MoveCursor(0, -1);
      } else if (c == 's'){
        Editor_MoveCursor(1, 0);
      } else if (c == 'd'){
        Editor_MoveCursor(0, 1);
      }
      else {
        Editor_SetCursor(TextFile_InsertChar(file, c, config.cursor));
        //TextFile_AppendChar(file, c);
        //Editor_MoveCursor(0, 1); 
      }
      TextFile_Print(file);
      Editor_PrintCursor();
      //https://stackoverflow.com/questions/26423537/how-to-position-the-input-text-cursor-in-c
      //https://stackoverflow.com/questions/50884685/how-to-get-cursor-position-in-c-using-ansi-code
    }

    return 0;
}

void Editor_MoveCursor(int row_change, int col_change){
  TextPos new_pos = {.x = config.cursor.x + col_change, .y = config.cursor.y + row_change};
  Editor_SetCursor(new_pos);
}

void Editor_SetCursor(TextPos pos){
  config.cursor.x = pos.x;
  config.cursor.y = pos.y;
  if (config.cursor.x < 0) config.cursor.x = 0;
  if (config.cursor.y < 1) config.cursor.y = 1;
  //don't allow the cursor to go past the end of the file
  if (config.cursor.y > config.file->num_lines){
    config.cursor.y = config.file->num_lines;
  }
  //don't allow the cursor to go past the end of the line
  if (config.cursor.x > config.file->lines[config.cursor.y - 1]->line_length){
    config.cursor.x = config.file->lines[config.cursor.y - 1]->line_length;
  }
}

void Editor_PrintCursor(){
  printf("\e[%d;%dH", config.cursor.y, config.cursor.x + 5);
}

void setup() { 
  // setup editor config
  config.file = TextFile_Allocate();
  config.cursor.x = 0;
  config.cursor.y = 1;
  config.mode = NORMAL;
  // enable raw mode
  setvbuf(stdout, NULL, _IONBF, 0);
  tcgetattr(STDIN_FILENO, &(config.orig_termios));
  atexit(finish);
  struct termios raw = config.orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void finish() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &(config.orig_termios));
}
