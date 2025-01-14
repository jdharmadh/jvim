#ifndef LAYOUT_H_
#define LAYOUT_H_

#include <stdbool.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 64
#define MAX_LINE_LENGTH 512
#define MAX_LINE_NUMBER 1024

typedef struct {
  char text[MAX_LINE_LENGTH];
  int16_t line_length;
} TextLine;

typedef struct {
  char* filename;
  TextLine* lines[MAX_LINE_NUMBER];
  int16_t num_lines;
} TextFile;

typedef struct {
  int16_t x;
  int16_t y;
} TextPos;

typedef struct {
  TextPos start;
  TextPos end;
} TextRange;

typedef struct SearchResult SearchResult;

typedef struct SearchResult {
  TextRange range;
  SearchResult* next;
} SearchResult;

typedef struct {
  char* find;
  char* replace;
  SearchResult* search_results;
  SearchResult* current_result;
} FindReplace;

enum EditorMode { NORMAL, INSERT, COMMAND, FIND_REPLACE };

typedef struct {
  char buf[MAX_COMMAND_LENGTH];
  int16_t idx;
} CommandBuffer;

typedef struct {
  struct termios orig_termios;
  TextFile* file;
  TextPos window_cursor;
  TextPos file_cursor;  // x represents columns past zero, y represents lines
                        // past zero
  struct winsize window_size;
  enum EditorMode mode;
  CommandBuffer* cmd_buf;
  FindReplace* find_replace;
  bool running;
} EditorConfig;

#endif  // LAYOUT_H_