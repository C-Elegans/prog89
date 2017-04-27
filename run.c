#include "run.h"
#include <stdio.h>
#include <string.h>
char* read_file_modes[] = {"w", "wb", "w"};

size_t get_file_size(FILE* file){
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

size_t min_z(size_t a, size_t b){
  return a > b ? b : a;
}

void handle_read(struct writeopt* opt){
  FILE* output;
  if(opt->filename && strlen(opt->filename) > 0 && opt->filetype != FT_IMMEDIATE){
    output = fopen(opt->filename, read_file_modes[opt->filetype]);
  } else {
    output = stdout;
  }
  uint8_t* buffer = malloc(options.memsize);
  for(int i=0;i<options.memsize/PAGE_SIZE;i++){
    pr_read_code_page(i,buffer+i*PAGE_SIZE);
  }
  switch(opt->filetype){
  case FT_RAW:
    fwrite(buffer, options.memsize, 1, output);
    fclose(output);
    break;
  default:
    fprintf(stderr,"Not implemented yet\n");
    exit(1);
    break;
  }
}

char* write_file_modes[] = {"", "rb", ""};

void handle_write(struct writeopt* opt){
  if(opt->filetype != FT_IMMEDIATE){
    FILE* input = fopen(opt->filename, write_file_modes[opt->filetype]);
    switch(opt->filetype){
    case FT_RAW:
      if(opt->memtype == MEM_FLASH){
	uint8_t* buffer = malloc(options.memsize);
	memset(buffer, 0xff, options.memsize);

	size_t readlen = min_z(options.memsize, get_file_size(input));
	fread(buffer, readlen, 1, input);
	for(int i=0;i<options.memsize/PAGE_SIZE;i++){
	  pr_write_code_page(i,buffer+i*PAGE_SIZE);
	}
      } else {
	fprintf(stderr, "Not implemented yet\n");
	exit(1);
      }
      break;
    default:
      fprintf(stderr, "Invalid filetype in write\n");
      exit(1);
    }

  } else {
    fprintf(stderr, "Not implemented yet\n");
    exit(1);
  }
}
void handle_verify(struct writeopt* opt){
}


void handle_option(struct writeopt* opt){
  switch(opt->op){
  case OP_READ:
    handle_read(opt);
    break;
  case OP_WRITE:
    handle_write(opt);
    break;
  case OP_VERIFY:
    handle_verify(opt);
    break;
  }
}

void run(void){
  if(options.chip_erase){
    pr_chip_erase();
  }
  for(int i=0;i<options.nwriteopts;i++){
    handle_option(options.writeopts[i]);
  }

}
