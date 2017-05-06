#ifndef _DEVICE_H
#define _DEVICE_H

struct device{
  int pagesize;
  int memsize;
  int needs_prefix;
};

struct device* device_from_string(char*);
int ilog2(int);

extern struct device at89lp213;




#endif
