#include "device.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

struct device at89lp213 = {
  .memsize = 2048,
  .pagesize = 32,
  .needs_prefix = 1,
};

struct device at89s52 = {
  .memsize = 8192,
  .pagesize = 64,
  .needs_prefix = 0,
};


struct device* device_from_string(char* str){
  if(strcasecmp(str,"at89lp213") == 0 || strcasecmp(str,"213") == 0){
    return &at89lp213;
  }
  else if(strcasecmp(str,"at89s52") == 0 || strcasecmp(str,"at89lp52") == 0 ||
	  strcasecmp(str,"52") == 0){
    return &at89s52;
  }
  else{
    fprintf(stderr, "Unknown device: %s\n",str);
    exit(1);
  }
}

int ilog2(int a){
  switch(a){
  case 1: return 0;
  case 2: return 1;
  case 4: return 2;
  case 8: return 3;
  case 16: return 4;
  case 32: return 5;
  case 64: return 6;
  case 128: return 7;
  case 256: return 8;
  default:
    fprintf(stderr,"ilog2: not a power of 2: %d\n", a);
    exit(1);

  }
  return -1;
}
