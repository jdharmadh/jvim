#ifndef EDITOR_H_
#define EDITOR_H_

#include "layouts.h"

EditorConfig *Editor_FreshSetup(char *filename);
void Editor_ProcessKey(EditorConfig *config, char c);
void Editor_ProcessEscape(EditorConfig *config);
void Editor_MoveCursor(EditorConfig *config, int row_change, int col_change);
void Editor_CursorToCurrentResult(EditorConfig *config);
void Editor_SetCursor(EditorConfig *config, TextPos pos);
void Editor_PrintCursor(EditorConfig *config);
void Editor_PrintHeader(EditorConfig *config);
void Editor_Print(EditorConfig *config);
void Editor_Free(EditorConfig *config);
void Editor_ProcessCommand(EditorConfig *config);

#endif  // EDITOR_H_