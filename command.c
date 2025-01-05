#include "command.h"

#include <stdlib.h>

CommandBuffer* CommandBuffer_Init() {
  CommandBuffer* cb = (CommandBuffer*)malloc(sizeof(CommandBuffer));
  cb->idx = 0;
  return cb;
}

void CommandBuffer_AddChar(CommandBuffer* cb, char c) {
  if (cb->idx < MAX_COMMAND_LENGTH) {
    cb->buf[cb->idx++] = c;
  }
}
void CommandBuffer_DeleteChar(CommandBuffer* cb) {
  if (cb->idx > 0) {
    cb->idx--;
  }
}

void CommandBuffer_Clear(CommandBuffer* cb) { cb->idx = 0; }