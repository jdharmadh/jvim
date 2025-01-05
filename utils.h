// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <string.h>

bool startsWith(char *prefix, char *str) {
  return strncmp(prefix, str, strlen(prefix)) == 0;
}

#endif  // UTILS_H
