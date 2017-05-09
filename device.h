#ifndef _DEVICE_H
#define _DEVICE_H
#include <stdint.h>
#include "command.h"
struct device{
  int pagesize;
  int memsize;
  int needs_prefix;
  int uses_half_page;
  uint8_t signature[3];
  struct command commands[COMMAND_MAX];
};

struct device* device_from_string(char*);
int ilog2(int);

extern struct device at89lp213;




#endif
