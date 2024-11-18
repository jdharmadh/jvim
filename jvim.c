#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 512
#define MAX_LINE_NUMBER 1024
#define RESETCOLOR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"

struct termios orig_termios;
typedef struct tline {
  char text[MAX_LINE_LENGTH];
  int16_t line_length;
} TextLine;

typedef struct tfile {
  TextLine* lines[MAX_LINE_NUMBER];
  int16_t num_lines;
} TextFile;

TextLine* TextLine_Allocate() {
  TextLine* tl = malloc(sizeof(TextLine));
  tl->line_length = 0;
  return tl;
}

TextFile* TextFile_Allocate() {
  TextFile* tf = malloc(sizeof(TextFile));
  tf->num_lines = 0;
  return tf;
}

TextLine* TextFile_AddLine(TextFile* file) {
  //if(file->num_lines == MAX_LINE_NUMBER) return;
  file->lines[file->num_lines] = malloc(sizeof(TextLine));
  return file->lines[file->num_lines++];
}

void TextFile_AddChar(TextFile* file, char c) {
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

void TextFile_DeleteChar(TextFile* file){
  if(file->num_lines == 0) return;
  TextLine* cur_line = file->lines[file->num_lines - 1];
  if(cur_line->line_length == 0) {
    free(cur_line);
    file->num_lines -= 1;
  } else {
    cur_line->line_length -= 1;
  }
}

void TextFile_Print(TextFile* file){
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

    // setup
    TextFile* file = TextFile_Allocate();

    TextFile_Print(file);
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1){
      if(c == 127 || c == 8){
        TextFile_DeleteChar(file);
      }
      else {
        TextFile_AddChar(file, c);
      }
      printf("\e[1;1H\e[2J");
      TextFile_Print(file);
      //https://stackoverflow.com/questions/26423537/how-to-position-the-input-text-cursor-in-c
      //https://stackoverflow.com/questions/50884685/how-to-get-cursor-position-in-c-using-ansi-code
    }

    return 0;
}
