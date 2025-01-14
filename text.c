#include "text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "colors.h"
#include "layouts.h"

TextLine* TextLine_Allocate() {
  TextLine* tl = (TextLine*)malloc(sizeof(TextLine));
  tl->line_length = 0;
  return tl;
}

TextLine* TextFile_AppendLine(TextFile* file) {
  file->lines[file->num_lines] = (TextLine*)malloc(sizeof(TextLine));
  return file->lines[file->num_lines++];
}

TextFile* TextFile_Setup(char* filename) {
  TextFile* tf = (TextFile*)malloc(sizeof(TextFile));
  tf->filename = malloc(strlen(filename) + 1);
  strcpy(tf->filename, filename);
  tf->num_lines = 0;
  TextFile_AppendLine(tf);
  // read in the file
  FILE* file = fopen(filename, "r");
  if (file) {
    char c;
    while ((c = fgetc(file)) != EOF) {
      TextFile_AppendChar(tf, c);
    }
    fclose(file);
  }
  return tf;
}

TextLine* TextFile_InsertLine(TextFile* file, int line_number) {
  if (file->num_lines == MAX_LINE_NUMBER) return NULL;
  for (int i = file->num_lines; i > line_number; i--) {
    file->lines[i] = file->lines[i - 1];
  }
  file->lines[line_number] = (TextLine*)malloc(sizeof(TextLine));
  file->num_lines++;
  return file->lines[line_number];
}

void TextFile_AppendChar(TextFile* file, char c) {
  if (c == 10) {
    TextFile_AppendLine(file);
    return;
  }
  TextLine* cur_line;
  if (file->num_lines == 0) {
    cur_line = TextFile_AppendLine(file);
  } else {
    cur_line = file->lines[file->num_lines - 1];
    if (cur_line->line_length == MAX_LINE_LENGTH) {
      cur_line = TextFile_AppendLine(file);
    }
  }
  cur_line->text[cur_line->line_length++] = c;
}

TextPos TextFile_InsertChar(TextFile* file, char c, TextPos pos) {
  TextPos new_pos = {.x = pos.x, .y = pos.y};
  TextLine* cur_line;
  if (file->num_lines == 0) {
    cur_line = TextFile_AppendLine(file);
  } else {
    // TODO: broken when we go over the max limit
    cur_line = file->lines[pos.y - 1];
    if (cur_line->line_length >= MAX_LINE_LENGTH - 1) {
      cur_line = TextFile_AppendLine(file);
    }
  }
  if (c == 10) {
    // insert a new line and move the rest of the line to the new line
    TextLine* next_line = TextFile_InsertLine(file, pos.y);
    for (int i = pos.x; i < cur_line->line_length; i++) {
      next_line->text[i - pos.x] = cur_line->text[i];
      // next_line->line_length++;
    }
    cur_line->text[pos.x] = c;
    next_line->line_length = cur_line->line_length - pos.x;
    new_pos.x = 0;
    new_pos.y += 1;
    cur_line->line_length = pos.x;
    return new_pos;
  }
  for (int i = cur_line->line_length; i > pos.x; i--) {
    cur_line->text[i] = cur_line->text[i - 1];
  }
  cur_line->text[pos.x] = c;
  cur_line->line_length++;
  new_pos.x += 1;
  return new_pos;
}

TextPos TextFile_DeleteChar(TextFile* file, TextPos pos) {
  // check if we are at the beginning of the file
  if (pos.x == 0 && pos.y == 1) return pos;
  // create a new pos
  TextPos new_pos = {.x = pos.x, .y = pos.y};
  TextLine* cur_line = file->lines[pos.y - 1];
  if (cur_line->line_length == 0) {
    free(cur_line);
    for (int i = pos.y; i < file->num_lines; i++) {
      file->lines[i - 1] = file->lines[i];
    }
    file->num_lines -= 1;

    // update new pos
    new_pos.x = file->lines[pos.y - 2]->line_length;
    new_pos.y -= 1;
    return new_pos;
  } else {
    // handle the case where we are at the beginning of the line
    if (pos.x == 0) {
      // move the rest of the line to the previous line
      TextLine* prev_line = file->lines[pos.y - 2];
      for (int i = prev_line->line_length; i < MAX_LINE_LENGTH; i++) {
        prev_line->text[i] = cur_line->text[i - prev_line->line_length];
      }
      new_pos.x = prev_line->line_length;
      prev_line->line_length += cur_line->line_length;
      free(cur_line);
      for (int i = pos.y; i < file->num_lines; i++) {
        file->lines[i - 1] = file->lines[i];
      }
      file->num_lines -= 1;

      // update new pos
      new_pos.y -= 1;
      return new_pos;
    }
    for (int i = pos.x - 1; i < cur_line->line_length; i++) {
      cur_line->text[i] = cur_line->text[i + 1];
    }
    cur_line->line_length -= 1;
    new_pos.x -= 1;
    return new_pos;
  }
}

void TextFile_Print(TextFile* file) {
  for (int i = 0; i < file->num_lines - 1; i++) {
    printf(GREEN);
    printf("%d   ", i + 1);
    printf(RESETCOLOR);
    printf("%.*s\n", file->lines[i]->line_length, file->lines[i]->text);
  }
  printf(GREEN);
  printf("%d   ", file->num_lines);
  printf(RESETCOLOR);
  printf("%.*s", file->lines[file->num_lines - 1]->line_length,
         file->lines[file->num_lines - 1]->text);
}

void TextFile_PrintLine(TextFile* file, int line_number) {
  printf(GREEN);
  printf("%d", line_number);
  if (line_number >= 100)
    printf(" ");
  else if (line_number >= 10)
    printf("  ");
  else
    printf("   ");
  printf(RESETCOLOR);
  printf("%.*s", file->lines[line_number - 1]->line_length,
         file->lines[line_number - 1]->text);
}

void TextFile_PrintLine_SearchMode(TextFile* file, int line_number,
                                   SearchResult* search_results,
                                   SearchResult* current_result) {
  printf(GREEN);
  printf("%d", line_number);
  if (line_number >= 100)
    printf(" ");
  else if (line_number >= 10)
    printf("  ");
  else
    printf("   ");
  printf(RESETCOLOR);
  TextLine* line = file->lines[line_number - 1];
  SearchResult* cur = search_results;
  int current_pos = 0;
  while (cur != NULL) {
    if (cur->range.start.y < line_number - 1) {
      cur = cur->next;
      continue;
    }
    if (cur->range.start.y > line_number - 1) break;

    // Print text before the match
    printf("%.*s", cur->range.start.x - current_pos, line->text + current_pos);
    // Print the match
    if (cur == current_result) {
      printf(BOLD);
      printf(MAGENTA);
    } else {
      printf(BOLD);
      printf(CYAN);
    }
    printf("%.*s", cur->range.end.x - cur->range.start.x,
           line->text + cur->range.start.x);
    printf(RESETCOLOR);
    printf(RESETFORMAT);
    current_pos = cur->range.end.x;
    cur = cur->next;
  }
  // Print remaining text after last match
  if (current_pos < line->line_length) {
    printf("%.*s", line->line_length - current_pos, line->text + current_pos);
  }
}

void TextFile_Save(TextFile* file) {
  FILE* opened_file = fopen(file->filename, "w");
  if (!opened_file) {
    fprintf(stderr, "Error opening file\n");
    exit(1);
  }
  // write the file
  for (int i = 0; i < file->num_lines - 1; i++) {
    fprintf(opened_file, "%.*s\n", file->lines[i]->line_length,
            file->lines[i]->text);
  }
  fprintf(opened_file, "%.*s", file->lines[file->num_lines - 1]->line_length,
          file->lines[file->num_lines - 1]->text);
  fclose(opened_file);
}

void TextFile_Free(TextFile* file) {
  for (int i = 0; i < file->num_lines; i++) {
    free(file->lines[i]);
  }
  free(file->filename);
  free(file);
}
