#include <stdlib.h>
#include <string.h>

#include "layouts.h"
#include "search.h"


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
            result->range.end.x = result->range.start.x + strlen(config->find_replace->find);
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
}