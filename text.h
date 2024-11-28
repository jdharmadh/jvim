#ifndef TEXT_H_
#define TEXT_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "layouts.h"
#include "colors.h"

TextLine* TextLine_Allocate() {
  TextLine* tl = (TextLine*) malloc(sizeof(TextLine));
  tl->line_length = 0;
  return tl;
}

TextFile* TextFile_Allocate() {
  TextFile* tf = (TextFile*) malloc(sizeof(TextFile));
  tf->num_lines = 0;
  return tf;
}

TextLine* TextFile_AddLine(TextFile* file) {
  //if(file->num_lines == MAX_LINE_NUMBER) return;
  file->lines[file->num_lines] = (TextLine*) malloc(sizeof(TextLine));
  return file->lines[file->num_lines++];
}

TextLine* TextFile_InsertLine(TextFile* file, int line_number) {
  if(file->num_lines == MAX_LINE_NUMBER) return NULL;
  for (int i = file->num_lines; i > line_number; i--){
    file->lines[i] = file->lines[i-1];
  }
  file->lines[line_number] = (TextLine*) malloc(sizeof(TextLine));
  file->num_lines++;
  return file->lines[line_number];
}

void TextFile_AppendChar(TextFile* file, char c) {
  if(c == 10){
    TextFile_AddLine(file);
    return;
  }
  TextLine* cur_line;
  if(file->num_lines == 0){
    cur_line = TextFile_AddLine(file);
  }
  else {
    cur_line = file->lines[file->num_lines - 1];
    if (cur_line->line_length == MAX_LINE_LENGTH){
      cur_line = TextFile_AddLine(file);
    }
  }
  cur_line->text[cur_line->line_length++] = c;
}

TextPos TextFile_InsertChar(TextFile* file, char c, TextPos pos){
  TextPos new_pos = {.x= pos.x, .y= pos.y};
  TextLine* cur_line;
  if(file->num_lines == 0){
    cur_line = TextFile_AddLine(file);
  }
  else {
    // broken when we go over the max limit
    cur_line = file->lines[pos.y - 1];
    if (cur_line->line_length >= MAX_LINE_LENGTH - 1){
      cur_line = TextFile_AddLine(file);
    }
  }
  if(c == 10){
    // insert a new line and move the rest of the line to the new line
    TextLine* next_line = TextFile_InsertLine(file, pos.y);
    for (int i = pos.x; i < cur_line->line_length; i++){
      next_line->text[i - pos.x] = cur_line->text[i];
      //next_line->line_length++;
    }
    cur_line->text[pos.x] = c;
    next_line->line_length = cur_line->line_length - pos.x;
    new_pos.x = 0;
    new_pos.y += 1;
    cur_line->line_length = pos.x;
    return new_pos;
  }
  for (int i = cur_line->line_length; i > pos.x; i--){
    cur_line->text[i] = cur_line->text[i-1];
  }
  cur_line->text[pos.x] = c;
  cur_line->line_length++;
  new_pos.x += 1;
  return new_pos;
}
/*
void TextFile_DeleteChar(TextFile* file){
  if(file->num_lines == 0) return;
  TextLine* cur_line = file->lines[file->num_lines - 1];
  if(cur_line->line_length == 0) {
    free(cur_line);
    file->num_lines -= 1;
  } else {
    cur_line->line_length -= 1;
  }
} */

TextPos TextFile_DeleteChar(TextFile* file, TextPos pos){
  if(file->num_lines == 0){
    return pos;
  }
  // create a new pos
  TextPos new_pos = {.x = pos.x, .y = pos.y};
  TextLine* cur_line = file->lines[pos.y - 1];
  if(cur_line->line_length == 0) {
    free(cur_line);
    for (int i = pos.y; i < file->num_lines; i++){
      file->lines[i-1] = file->lines[i];
    }
    file->num_lines -= 1;
    new_pos.y -= 1;
    return new_pos;
  } else {
    //handle the case where we are at the beginning of the line
    if(pos.x == 0){
      // check if we are at the beginning of the file
      if(pos.y == 1) return new_pos;
      // move the rest of the line to the previous line
      TextLine* prev_line = file->lines[pos.y - 2];
      for (int i = prev_line->line_length; i < MAX_LINE_LENGTH; i++){
        prev_line->text[i] = cur_line->text[i - prev_line->line_length];
      }
      new_pos.x = prev_line->line_length + 1;
      prev_line->line_length += cur_line->line_length;
      free(cur_line);
      for (int i = pos.y; i < file->num_lines; i++){
        file->lines[i-1] = file->lines[i];
      }
      file->num_lines -= 1;
      // update new pos
      new_pos.y -= 1;
      return new_pos;
    }
    for (int i = pos.x - 1; i < cur_line->line_length; i++){
      cur_line->text[i] = cur_line->text[i+1];
    }
    cur_line->line_length -= 1;
    //new_pos.x -= 1;
    return new_pos;
  }
}

void TextFile_Print(TextFile* file){
  printf("\e[1;1H\e[2J");
  if(file->num_lines == 0){
    printf(GREEN);
    printf("1   ");
    printf(RESETCOLOR);
    return;
  } 
  for (int i = 0; i < file->num_lines-1; i++) {
    printf(GREEN);
    printf("%d   ", i+1);
    printf(RESETCOLOR);
    printf("%.*s\n", file->lines[i]->line_length, file->lines[i]->text);
  }
  printf(GREEN);
  printf("%d   ", file->num_lines);
  printf(RESETCOLOR);
  printf("%.*s", file->lines[file->num_lines-1]->line_length, file->lines[file->num_lines-1]->text);
}

#endif // TEXT_H_