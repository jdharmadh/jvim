#ifndef COMMAND_H_
#define COMMAND_H_

#include "layouts.h"

CommandBuffer* CommandBuffer_Init();
void CommandBuffer_AddChar(CommandBuffer* cb, char c);
void CommandBuffer_DeleteChar(CommandBuffer* cb);
void CommandBuffer_Clear(CommandBuffer* cb);

#endif // COMMAND_H_