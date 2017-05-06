#include "run.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ihex.h"
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
  printf("Reading\n");
  FILE* output;
  if(opt->filename && strlen(opt->filename) > 0 && opt->filetype != FT_IMMEDIATE){
    output = fopen(opt->filename, read_file_modes[opt->filetype]);
  } else {
    output = stdout;
  }
  uint8_t* buffer = malloc(options.device->memsize);
  for(int i=0;i<options.device->memsize/PAGE_SIZE;i++){
    pr_read_code_page(i,buffer+i*PAGE_SIZE);
  }
  switch(opt->filetype){
  case FT_RAW:
    fwrite(buffer, options.device->memsize, 1, output);
    fclose(output);
    break;
  case FT_IMMEDIATE:
    print_buffer(buffer, options.device->memsize);
    break;
  default:
    fprintf(stderr,"Not implemented yet\n");
    exit(1);
    break;
  }
}

char* write_file_modes[] = {0, "rb", "rb", 0};

int read_file(struct writeopt* opt, uint8_t** buffer, size_t* len){
  FILE* input;
  if(opt->filetype != FT_IMMEDIATE){
    input = fopen(opt->filename, write_file_modes[opt->filetype]);
    if(!input){
      perror("Error opening file");
    }
  }
  switch(opt->filetype){
  case FT_RAW:{
    *buffer = malloc(options.device->memsize);
    memset(*buffer, 0xff, options.device->memsize);
    *len = min_z(options.device->memsize, get_file_size(input));
    fread(*buffer, *len, 1, input);
    return 0;
  }
  case FT_IHEX:
    return parse_ihex(input, buffer, len);
  default:
    return 1;
  }

}

void handle_write(struct writeopt* opt){
  if(opt->memtype == MEM_FLASH){
    uint8_t* buffer = NULL;
    size_t readlen = 0;
    if(read_file(opt,&buffer, &readlen) != -1){
      assert(readlen != 0);
      assert(buffer != NULL);
      readlen = (readlen + PAGE_SIZE - 1) & ~(PAGE_SIZE-1); //align to page size
      for(size_t i=0;i<readlen/PAGE_SIZE;i++){
	pr_write_code_page(i,buffer+i*PAGE_SIZE);
      }
    } else {
      fprintf(stderr, "Error in read_file\n");
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
  uint8_t* sig = pr_read_atmel_signature();
  print_buffer(sig,PAGE_SIZE);
  free(sig);
  if(options.chip_erase){
    pr_chip_erase();
  }
  for(int i=0;i<options.nwriteopts;i++){
    handle_option(options.writeopts[i]);
  }

}
