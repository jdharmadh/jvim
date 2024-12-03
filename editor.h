#ifndef EDITOR_H_
#define EDITOR_H_

#include <stdbool.h>

#include "layouts.h"
#include "text.h"
#include "colors.h"

EditorConfig* Editor_Setup();
void Editor_ProcessKey(EditorConfig *config, char c);
void Editor_ProcessEscape(EditorConfig *config);
void Editor_MoveCursor(EditorConfig *config, int row_change, int col_change);
void Editor_SetCursor(EditorConfig *config, TextPos pos);
void Editor_PrintCursor(EditorConfig *config);
void Editor_PrintHeader(EditorConfig *config);
void Editor_Print(EditorConfig *config);

EditorConfig* Editor_Setup() {
  EditorConfig* ec = (EditorConfig*) malloc(sizeof(EditorConfig));
  ec->file = TextFile_Setup();
  ec->cursor.x = 0;
  ec->cursor.y = 1;
  ec->mode = NORMAL;
  ec->running = true;
  return ec;
}

void Editor_ProcessKey(EditorConfig *config, char c) {
  // check if we are moving the cursor
  if (c == 27){
    Editor_ProcessEscape(config);
    return;
  }
  // check if we are in insert mode
  if (config->mode == INSERT) {
    if(c == 127 || c == 8){
        Editor_SetCursor(config, TextFile_DeleteChar(config->file, config->cursor));
      } else {
        Editor_SetCursor(config, TextFile_InsertChar(config->file, c, config->cursor));
    }
  } else {
    // if the user pressed the i key, enter insert mode
    if (c == 'i'){
      config->mode = INSERT;
    } else if (c == 'q'){
      config->running = false;
    } else if (c == '0'){
      config->cursor.x = 0;
    } else if (c == '$'){
      config->cursor.x = config->file->lines[config->cursor.y - 1]->line_length;
    } else if (c == 'G'){
      config->cursor.y = config->file->num_lines;
    } else if (c == 'o'){
      TextFile_InsertLine(config->file, config->cursor.y);
      config->cursor.y += 1;
      config->cursor.x = 0;
    } else if (c == 'O'){
      TextFile_InsertLine(config->file, config->cursor.y - 1);
      config->cursor.x = 0;
    }
  }
}

void Editor_ProcessEscape(EditorConfig *config){
  char c;
  int prev_mode = config->mode;
  //if the user just pressed the escape key, return to normal mode
  config->mode = NORMAL;
  Editor_Print(config);
  if (read(STDIN_FILENO, &c, 1) == 1){
    if (c == '['){
      config->mode = prev_mode;
      if (read(STDIN_FILENO, &c, 1) == 1){
        if (c == 'A'){
          Editor_MoveCursor(config, -1, 0);
        } else if (c == 'B'){
          Editor_MoveCursor(config, 1, 0);
        } else if (c == 'C'){
          Editor_MoveCursor(config, 0, 1);
        } else if (c == 'D'){
          Editor_MoveCursor(config, 0, -1);
        }
      }
    } else {
      Editor_ProcessKey(config, c);
    }
  }
}

void Editor_MoveCursor(EditorConfig *config, int row_change, int col_change){
  TextPos new_pos = {.x = config->cursor.x + col_change, .y = config->cursor.y + row_change};
  Editor_SetCursor(config, new_pos);
}

void Editor_SetCursor(EditorConfig *config, TextPos pos){
  config->cursor.x = pos.x;
  config->cursor.y = pos.y;
  if (config->cursor.x < 0){
    if (config->cursor.y > 1){
      config->cursor.y -= 1;
      config->cursor.x = config->file->lines[config->cursor.y - 1]->line_length;
    } else {
      config->cursor.x = 0;
    }
  }
  if (config->cursor.y < 1) config->cursor.y = 1;
  //don't allow the cursor to go past the end of the file
  if (config->cursor.y > config->file->num_lines){
    config->cursor.y = config->file->num_lines;
  }
  //don't allow the cursor to go past the end of the line
  if (config->cursor.x > config->file->lines[config->cursor.y - 1]->line_length){
    config->cursor.x = config->file->lines[config->cursor.y - 1]->line_length;
  }
}

void Editor_PrintCursor(EditorConfig *config){
  printf("\e[%d;%dH", config->cursor.y + 1, config->cursor.x + 5);
}

void Editor_PrintHeader(EditorConfig *config){
  
  if (config->mode == INSERT){
    printf(YELLOW);
    printf("-- INSERT --");
    printf(RESETCOLOR);
  }
}

void Editor_Print(EditorConfig *config){
  printf("\e[1;1H\e[2J");
  Editor_PrintHeader(config);
  printf("\n");
  TextFile_Print(config->file);
  Editor_PrintCursor(config);
}

#endif // EDITOR_H_