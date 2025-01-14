#include "search.h"

#include <stdlib.h>
#include <string.h>

#include "editor.h"
#include "layouts.h"
#include "search.h"

// TODO: fix memory leaks, need to destroy search results

void Search_Find(EditorConfig* config) {
  SearchResult* head = NULL;
  SearchResult* tail = NULL;
  for (int i = 0; i < config->file->num_lines; i++) {
    TextLine* line = config->file->lines[i];
    char* found = strstr(line->text, config->find_replace->find);
    while (found != NULL) {
      SearchResult* result = malloc(sizeof(SearchResult));
      result->range.start.x = found - line->text;
      result->range.start.y = i;
      result->range.end.x =
          result->range.start.x + strlen(config->find_replace->find);
      result->range.end.y = i;
      if (head == NULL) {
        head = result;
        tail = result;
      } else {
        tail->next = result;
        tail = result;
      }
      found = strstr(found + 1, config->find_replace->find);
    }
  }

  config->find_replace->search_results = head;
  config->find_replace->current_result = head;
}

void Search_ReplaceAll(EditorConfig* config) {
  while (config->find_replace->search_results != NULL) {
    Search_ReplaceNextResult(config);
  }
  config->mode = NORMAL;
  Editor_Print(config);
}

void Search_ReplaceNextResult(EditorConfig* config) {
  if (config->find_replace->current_result == NULL) return;

  TextRange range = config->find_replace->current_result->range;
  TextLine* line = config->file->lines[range.start.y];
  int find_len = strlen(config->find_replace->find);
  int replace_len = strlen(config->find_replace->replace);
  int diff = replace_len - find_len;

  // Create a new buffer for the line text
  char* new_text = malloc(line->line_length + diff + 1);

  // Copy text before the match
  strncpy(new_text, line->text, range.start.x);

  // Copy the replacement text
  strcpy(new_text + range.start.x, config->find_replace->replace);

  // Copy text after the match
  strcpy(new_text + range.start.x + replace_len, line->text + range.end.x);

  // Update the line text
  strcpy(line->text, new_text);
  free(new_text);
  line->line_length += diff;

  // Update search results
  if (config->find_replace->current_result ==
      config->find_replace->search_results) {
    config->find_replace->search_results =
        config->find_replace->search_results->next;
    free(config->find_replace->current_result);
    config->find_replace->current_result = config->find_replace->search_results;
    return;
  } else {
    SearchResult* cur = config->find_replace->search_results;
    while (cur->next != config->find_replace->current_result) {
      cur = cur->next;
    }
    cur->next = config->find_replace->current_result->next;
    free(config->find_replace->current_result);
    config->find_replace->current_result = cur->next;
  }
  if (config->find_replace->current_result == NULL) {
    config->find_replace->current_result = config->find_replace->search_results;
  }
}