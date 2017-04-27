#ifndef _WRITEOPT_H
#define _WRITEOPT_H
enum memtype{
  MEM_FLASH=0,
  MEM_FUSE,
  MEM_LOCK,
  MEM_SIGNATURE,
  
};
enum op{
  OP_READ,
  OP_WRITE,
  OP_VERIFY
};
enum filetype{
  FT_IMMEDIATE,
  FT_RAW,
  FT_HEX, // Output only
};
struct writeopt{
  char* filename;
  enum memtype memtype;
  enum op op;
  enum filetype filetype;
};

struct writeopt* decode_writeopt(char* opt); /* Will modify opt */
#endif
