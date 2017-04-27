#include "programmer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct mpsse_context *context = NULL;
#define RESET_PIN GPIOL0
uint8_t default_fuses[32] = {
  0xff, 0xff,    //Internal RC Oscillator
  0xff, 0xff,    //4ms startup time
  0xff, //reset enable
  0xff, //BOD enable
  0xff, //On Chip Debug disable
  0xff, //ISP enable
  0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, //default oscillator setting
  0x00, //user signature programming enabled
  0xff, //I/Os start in input mode
  0x00, //No OCD interface
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Rest of the fuses (unused)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void pr_init(void){
  if((context = MPSSE(SPI0, TWO_MHZ, MSB)) == NULL || ! context->open){
    fprintf(stderr,"Failed to initialize MPSSE: %s\n", ErrorString(context));
    exit(1);
  }
  printf("%s initialized at %dHz (SPI 0)\n", GetDescription(context), GetClock(context));
  //Reset MCU
  PinLow(context,RESET_PIN);
}
void pr_destroy(void){
  if(context && context->open)
    PinHigh(context,RESET_PIN);
  Close(context);
  
}
void pr_run_command_rd(enum opcode cmd,
		       uint8_t* addrdata,
		       size_t addrsize,
		       uint8_t* retbuf,
		       size_t retbufsize){
  char cmdbuf[3] ={0xAA, 0x55, (char) cmd};
  Start(context);
  Write(context, cmdbuf , 3);
  Write(context, (char*) addrdata, addrsize);
  if(retbufsize){
    char* buf = Read(context, retbufsize);
    memcpy(retbuf,buf,retbufsize);
    free(buf);
  }
  Stop(context);
 
}

void print_buffer(uint8_t* buffer, size_t len){
  for(size_t i=0;i<len;i++)
    printf("%02x ", buffer[i]);
  printf("\n");
}

void pr_enable_program_mode(void){
  uint8_t buf[1] = {0b01010011};
  pr_run_command_rd(PROGRAM_ENABLE, buf, sizeof(buf), NULL,0);
}
uint8_t* pr_read_atmel_signature(void){
  uint8_t buf[2] = {0,0};
  uint8_t ret[32] = {0};
  pr_run_command_rd(READ_ATMEL_SIGNATURE_PAGE, buf, sizeof(buf),ret,sizeof(ret));
  print_buffer(ret,sizeof(ret));
  uint8_t* ret_buf = malloc(sizeof(ret));
  memcpy(ret_buf,ret,sizeof(ret));
  return ret_buf;

}

uint8_t* pr_read_user_fuses(void){
  uint8_t buf[2] = {0,0};
  uint8_t ret[0x13] = {0};
  pr_run_command_rd(READ_USER_FUSES, buf, sizeof(buf),ret,sizeof(ret));
  print_buffer(ret,sizeof(ret));
  return NULL;
}

void pr_write_user_fuses(uint8_t* fuses){
  uint8_t buf[2+0x13] = {0,0};
  memcpy(buf+2,fuses,0x13);
  print_buffer(buf,sizeof(buf));
  //pr_run_command(WRITE_USER_FUSES_ERASE, buf, sizeof(buf));
  //pr_run_command(READ_USER_FUSES, buf, sizeof(buf));
}
