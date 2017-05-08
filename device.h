#ifndef _DEVICE_H
#define _DEVICE_H
#include "command.h"
struct device{
  int pagesize;
  int memsize;
  int needs_prefix;
  struct command commands[COMMAND_MAX];
};

struct device* device_from_string(char*);
int ilog2(int);

extern struct device at89lp213;




#endif
