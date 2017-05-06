#include "ihex.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "programmer.h"

size_t max_z(size_t a, size_t b){
  return a>b?a:b;
}

uint8_t ival(char c){
  if(c >= '0' && c <= '9'){
    return c - '0';
  }
  if(c >= 'a' && c <= 'f'){
    return c - 'a' + 0xa;
  }
  if(c >= 'A' && c <= 'F'){
    return c - 'A' + 0xa;
  }
  return 0xff;

}

uint32_t readhex(char* str, int nchars){
  assert(nchars < 8 && nchars > 0);
  uint32_t sum = 0;
  while(nchars != 0){
    sum = sum << 4;
    sum |= ival(*str++);
    nchars--;
  }
  return sum;
}

enum record_state{
  STATE_INITIAL,
  STATE_BYTE_COUNT,
  STATE_ADDR,
  STATE_REC_TYPE,
  STATE_DATA,
  STATE_CHECKSUM,
  
};

//record state machine
int parse_record(char* line, uint8_t* buffer, size_t* buflen){
  enum record_state state = STATE_INITIAL;
  size_t byte_count = 0;
  size_t addr = 0;
  int rec_type = 0;
  uint8_t checksum = 0;
  while(*line){
    switch(state){
    case STATE_INITIAL:
      if(*line == ':'){
	state = STATE_BYTE_COUNT;
      }
      line++;
      break;

    case STATE_BYTE_COUNT:
      byte_count = readhex(line, 2);
      checksum += readhex(line,2);
      line += 2;
      state = STATE_ADDR;
      break;
    case STATE_ADDR:
      addr = readhex(line,4);
      checksum += readhex(line,2);
      checksum += readhex(line+2,2);
      buffer = buffer+addr;
      *buflen = max_z(*buflen, addr);
      line += 4;
      state = STATE_REC_TYPE;
      break;
    case STATE_REC_TYPE:
      rec_type = readhex(line,2);
      checksum += readhex(line,2);
      line += 2;
      switch(rec_type){
      case 0:
	if(byte_count){
	  state = STATE_DATA; break;
	} else {
	  state = STATE_CHECKSUM; break;
	}
      case 1: return 1;
      default:
	fprintf(stderr, "Invalid record type: %d\n",rec_type); exit(1); break;
      }
      break;
    case STATE_DATA:
      if(byte_count--){
	*buffer++ = readhex(line,2);
	checksum += readhex(line,2);
	*buflen += 1;
	line +=2;
      } else {
	state = STATE_CHECKSUM;
      }
      break;
    case STATE_CHECKSUM:
      checksum += readhex(line,2);
      if(checksum != 0){
	fprintf(stderr,"Invalid checksum! %X %s\n",checksum, line);
	exit(1);
      }
      return 0;
      break;
    default:
      line++;
      break;
    }
    
  }
  return 0;
}

int parse_ihex(FILE* file, uint8_t** ret_buf, size_t* ret_len){
  uint8_t* buffer = malloc(options.device->memsize);
  memset(buffer,0xff,options.device->memsize);
  uint8_t* current = buffer;
  size_t buflen = 0;
  char* line = NULL;
  size_t linelen = 0;
  ssize_t read;
  while((read = getline(&line, &linelen, file)) != -1) {
    if(parse_record(line, current, &buflen) != 0) break;
  }
  free(line);
  *ret_buf = buffer;
  *ret_len = buflen;
  return 0;
}
