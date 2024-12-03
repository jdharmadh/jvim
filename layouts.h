#ifndef LAYOUT_H_
#define LAYOUT_H_

#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 512
#define MAX_LINE_NUMBER 1024

typedef struct tline {
  char text[MAX_LINE_LENGTH];
  int16_t line_length;
} TextLine;

typedef struct tfile {
  TextLine* lines[MAX_LINE_NUMBER];
  int16_t num_lines;
} TextFile;

// This is not the actual position in the terminal because
// we have the line numbers which offset the actual position by 4
typedef struct tpos {
  int16_t x;
  int16_t y;
} TextPos;

enum EditorMode {
  NORMAL,
  INSERT,
  COMMAND
};

typedef struct editorConfig {
  struct termios orig_termios;
  TextFile* file;
  TextPos cursor;
  enum EditorMode mode;
  bool running;
} EditorConfig;

#endif // LAYOUT_H_