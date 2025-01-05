#ifndef TEXT_H_
#define TEXT_H_

#include "layouts.h"

TextLine* TextLine_Allocate();
TextLine* TextFile_AppendLine(TextFile* file);
TextFile* TextFile_Setup(char* filename);
TextLine* TextFile_InsertLine(TextFile* file, int line_number);
void TextFile_AppendChar(TextFile* file, char c);
TextPos TextFile_InsertChar(TextFile* file, char c, TextPos pos);
TextPos TextFile_DeleteChar(TextFile* file, TextPos pos);
void TextFile_Print(TextFile* file);
void TextFile_PrintLine(TextFile* file, int line_number);
void TextFile_PrintLine_SearchMode(TextFile* file, int line_number,
                                   SearchResult* search_results);
void TextFile_Free(TextFile* file);
void TextFile_Save(TextFile* file);

#endif  // TEXT_H_