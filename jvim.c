#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "layouts.h"
#include "text.h"
#include "colors.h"

EditorConfig config;

void setup();
void finish();
void Editor_MoveCursor(int row_change, int col_change);
void Editor_SetCursor(TextPos pos);
void Editor_PrintCursor();
void Editor_PrintHeader();
void Editor_Print();
void Editor_ProcessEscape();
void Editor_ProcessKey(char c);

int main(int argc, char **argv){
    setup();
    // setup
    char c;
    while (config.running && read(STDIN_FILENO, &c, 1) == 1){
      // some asserts for debugging
      assert(config.running);
      assert(config.cursor.x >= 0);
      assert(config.cursor.y >= 1);
      assert(config.file->num_lines > 0);
      assert(config.cursor.y <= config.file->num_lines + 1);
      assert(config.cursor.x <= config.file->lines[config.cursor.y - 1]->line_length);
      Editor_ProcessKey(c);
      Editor_Print();
    }
    finish();
    return 0;
}

void Editor_ProcessKey(char c) {
  // check if we are moving the cursor
  if (c == 27){
    Editor_ProcessEscape();
    return;
  }
  // check if we are in insert mode
  if (config.mode == INSERT) {
    if(c == 127 || c == 8){
        Editor_SetCursor(TextFile_DeleteChar(config.file, config.cursor));
      } else {
        Editor_SetCursor(TextFile_InsertChar(config.file, c, config.cursor));
    }
  } else {
    // if the user pressed the i key, enter insert mode
    if (c == 'i'){
      config.mode = INSERT;
    } else if (c == 'q'){
      config.running = false;
    } else if (c == '0'){
      config.cursor.x = 0;
    } else if (c == '$'){
      config.cursor.x = config.file->lines[config.cursor.y - 1]->line_length;
    } else if (c == 'G'){
      config.cursor.y = config.file->num_lines;
    } else if (c == 'o'){
      TextFile_InsertLine(config.file, config.cursor.y);
      config.cursor.y += 1;
      config.cursor.x = 0;
    } else if (c == 'O'){
      TextFile_InsertLine(config.file, config.cursor.y - 1);
      config.cursor.x = 0;
    }
  }
}

void Editor_ProcessEscape(){
  char c;
  int prev_mode = config.mode;
  //if the user just pressed the escape key, return to normal mode
  config.mode = NORMAL;
  Editor_Print();
  if (read(STDIN_FILENO, &c, 1) == 1){
    if (c == '['){
      config.mode = prev_mode;
      if (read(STDIN_FILENO, &c, 1) == 1){
        if (c == 'A'){
          Editor_MoveCursor(-1, 0);
        } else if (c == 'B'){
          Editor_MoveCursor(1, 0);
        } else if (c == 'C'){
          Editor_MoveCursor(0, 1);
        } else if (c == 'D'){
          Editor_MoveCursor(0, -1);
        }
      }
    } else {
      Editor_ProcessKey(c);
    }
  }
}

void Editor_MoveCursor(int row_change, int col_change){
  TextPos new_pos = {.x = config.cursor.x + col_change, .y = config.cursor.y + row_change};
  Editor_SetCursor(new_pos);
}

void Editor_SetCursor(TextPos pos){
  config.cursor.x = pos.x;
  config.cursor.y = pos.y;
  if (config.cursor.x < 0){
    if (config.cursor.y > 1){
      config.cursor.y -= 1;
      config.cursor.x = config.file->lines[config.cursor.y - 1]->line_length;
    } else {
      config.cursor.x = 0;
    }
  }
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
  printf("\e[%d;%dH", config.cursor.y + 1, config.cursor.x + 5);
}

void Editor_PrintHeader(){
  
  if (config.mode == INSERT){
    printf(YELLOW);
    printf("-- INSERT --");
    printf(RESETCOLOR);
  }
}
void Editor_Print(){
  printf("\e[1;1H\e[2J");
  Editor_PrintHeader();
  printf("\n");
  TextFile_Print(config.file);
  Editor_PrintCursor();
}

void setup() { 
  // setup editor config
  config.file = TextFile_Setup();
  config.cursor.x = 0;
  config.cursor.y = 1;
  config.mode = NORMAL;
  config.running = true;
  // enable raw mode
  setvbuf(stdout, NULL, _IONBF, 0);
  tcgetattr(STDIN_FILENO, &(config.orig_termios));
  struct termios raw = config.orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  Editor_Print();
}

void finish() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &(config.orig_termios));
}
