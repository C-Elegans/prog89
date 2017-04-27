#include "writeopt.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void error(const char* fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr,fmt,ap);
  va_end(ap);
  exit(1);

}

char* memory_type_table[] = {"flash","fuse","lock","signature"};
enum memtype decode_memtype(char* str){
  for(size_t i=0;i<sizeof(memory_type_table)/sizeof(memory_type_table[0]);i++){
    if(strcmp(str,memory_type_table[i]) == 0){
      return i;
    }

  }
  fprintf(stderr, "Unrecognized memory type: %s\n", str);
  exit(1);
}

enum op decode_op(char* str){
  if(strlen(str) != 1){
    error("Invalid op: %s\n", str);
  }
  switch(str[0]){
  case 'r': return OP_READ;
  case 'w': return OP_WRITE;
  case 'v': return OP_VERIFY;
  default: error("Invalid op: %c\n", str[0]);
  }
  return 0;
}

enum filetype decode_filetype(char* str){
  if(strlen(str) != 1){
    error("Invalid format: %s\n", str);
  }
  switch(str[0]){
  case 'r': return FT_RAW;
  case 'i': return FT_IMMEDIATE;
  case 'h': return FT_HEX;
  default:
    error("Invalid format: %c\n",str[0]);
  }
  return 0;
}

struct writeopt* decode_writeopt(char* opt){
  struct writeopt* writeopt = malloc(sizeof(struct writeopt));
  char *token;
  token = strtok(opt,":");
  if(!token){error("Missing memtype in -U memtype:op:filename:format\n");}
  writeopt->memtype = decode_memtype(token);
  token = strtok(NULL,":");
  if(!token){error("Missing op specifier in -U memtype:op:filename:format\n");}
  writeopt->op = decode_op(token);
  token = strtok(NULL,":");
  if(!token){error("Missing filename in -U memtype:op:filename:format\n");}
  writeopt->filename = strdup(token);
  token = strtok(NULL,":");
  if(!token){error("Missing format in -U memtype:op:filename:format\n");}
  writeopt->filetype = decode_filetype(token);
  token = strtok(NULL,":");
  if(token){error("Too many options to -U\n");}
  return writeopt;
}
