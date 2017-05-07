#include "run.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ihex.h"
char* read_file_modes[] = {NULL, "wb", "w", "w"};

size_t get_file_size(FILE* file){
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

size_t min_z(size_t a, size_t b){
  return a > b ? b : a;
}

void write_file(uint8_t* buffer, size_t size, struct writeopt* opt){
  FILE* output;
  if(opt->filename && strlen(opt->filename) > 0 && opt->filetype != FT_IMMEDIATE){
    output = fopen(opt->filename, read_file_modes[opt->filetype]);
  } else {
    output = stdout;
  }
  switch(opt->filetype){
  case FT_RAW:
    fwrite(buffer, size, 1, output);
    fclose(output);
    break;
  case FT_IMMEDIATE:
    print_buffer(buffer, size);
    break;
  default:
    fprintf(stderr,"Not implemented yet\n");
    exit(1);
    break;
  }
}

void handle_read(struct writeopt* opt){
  uint8_t* buffer;
  switch(opt->memtype){
  case MEM_FLASH:
    buffer = malloc(options.memsize);
    for(int i=0;i<options.memsize/PAGE_SIZE;i++){
      pr_read_code_page(i,buffer+i*PAGE_SIZE);
    }
    write_file(buffer, options.memsize, opt);
    free(buffer);
    break;
  case MEM_FUSE:
    buffer = malloc(PAGE_SIZE);
    pr_read_user_fuses(buffer);
    write_file(buffer, PAGE_SIZE, opt);
    free(buffer);
    break;
  case MEM_LOCK:
    buffer = malloc(PAGE_SIZE);
    pr_read_lock_bits(buffer);
    write_file(buffer, PAGE_SIZE, opt);
    free(buffer);
    break;
  case MEM_SIGNATURE:
    buffer = malloc(2*PAGE_SIZE);
    pr_read_user_signature(buffer);
    write_file(buffer, PAGE_SIZE*2, opt);
    free(buffer);
    break;
    // No default to give warnings for unimplemented memory types
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
  case FT_IMMEDIATE:{
    *len = strlen(opt->filename)/2;
    *len = (*len + PAGE_SIZE - 1) & ~(PAGE_SIZE-1);
    *buffer = malloc(*len);
    memset(*buffer, 0xff, *len);
    size_t index = 0;
    char* str = opt->filename;
    while(index < *len && *str != 0 && *(str+1) != 0){
      (*buffer)[index++] = readhex(str, 2);
      str += 2;
    }
    return 0;
  }
    
  case FT_IHEX:
    return parse_ihex(input, buffer, len);
  default:
    return 1;
  }

}

void handle_write(struct writeopt* opt){
  uint8_t* buffer;
  size_t readlen;
  if(read_file(opt,&buffer, &readlen) != -1){
    switch(opt->memtype){
    case MEM_FLASH:
      readlen = (readlen + PAGE_SIZE - 1) & ~(PAGE_SIZE-1); //align to page size
      for(size_t i=0;i<readlen/PAGE_SIZE;i++){
	pr_write_code_page(i,buffer+i*PAGE_SIZE);
      }
      break;
    case MEM_FUSE:
      readlen = (readlen + PAGE_SIZE - 1) & ~(PAGE_SIZE-1);
      pr_write_user_fuses(buffer);
      break;
    case MEM_SIGNATURE:{
      uint8_t* buf2 = malloc(2*PAGE_SIZE);
      memset(buf2, 0xff, 2*PAGE_SIZE);
      memcpy(buf2, buffer, readlen);
      readlen = 2*PAGE_SIZE;
      print_buffer(buf2, readlen);
      pr_write_user_signature(buf2);
      free(buf2);
      break;
    }
    default:
      fprintf(stderr, "Not implemented yet\n");
      exit(1);
      break;
    }
    free(buffer);
  } else {
    fprintf(stderr, "Error in read_file\n");
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
