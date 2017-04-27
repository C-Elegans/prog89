#include "programmer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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
  if(addrsize){
    Write(context, (char*) addrdata, addrsize);
  }
  if(retbufsize){
    char* buf = Read(context, retbufsize);
    memcpy(retbuf,buf,retbufsize);
    free(buf);
  }
  Stop(context);
 
}
void pr_run_command_wr(enum opcode cmd,
		       uint8_t* addrdata,
		       size_t addrsize,
		       uint8_t* writebuf,
		       size_t writebufsize){
  char cmdbuf[3] = {0xAA, 0x55, (char) cmd};
  Start(context);
  Write(context, cmdbuf, sizeof(cmdbuf));
  if(addrsize){
    Write(context, (char*) addrdata, addrsize);
  }
  if(writebufsize){
    Write(context, (char*) writebuf, writebufsize);
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
  uint8_t buf[2] = {0,0};
  uint8_t data[0x13];
  memcpy(data,fuses,0x13);
  print_buffer(data,sizeof(data));
  pr_run_command_wr(WRITE_USER_FUSES_ERASE, buf, sizeof(buf), data, sizeof(data));
  while(!(pr_read_status() & 1)) {usleep(100);} //wait until write finishes
}

void pr_chip_erase(void){
  pr_run_command_rd(CHIP_ERASE, NULL, 0, NULL, 0);
}

uint8_t pr_read_status(void){
  uint8_t status_ret;
  pr_run_command_rd(READ_STATUS, &status_ret, sizeof(status_ret), NULL, 0);
  printf("0x%02x\n",status_ret);
  return status_ret;
}

void pr_read_code_page(int pagenum, uint8_t* buffer){
  assert(pagenum < 64 && pagenum >= 0);
  uint8_t addr_buf[2];
  addr_buf[0] = (pagenum >> 3) & 0x7;
  addr_buf[1] = (pagenum << 5) & 0xff;
  pr_run_command_rd(READ_CODE_PAGE, addr_buf, sizeof(addr_buf), buffer, 32);
}

void pr_write_code_page(int pagenum, uint8_t* buffer){
  printf("Write code page\n");
  assert(pagenum < 64 && pagenum >= 0);
  uint8_t addr_buf[2];
  addr_buf[0] = (pagenum >> 3) & 0x7;
  addr_buf[1] = (pagenum << 5) & 0xff;
  if(buffer)
    pr_run_command_wr(WRITE_CODE_PAGE_ERASE, addr_buf, sizeof(addr_buf), buffer, 32);
  else
    pr_run_command_wr(WRITE_CODE_PAGE_ERASE, addr_buf, sizeof(addr_buf), NULL, 0);
  usleep(5500); //5ms = write cycle time with auto-erase
}

//Don't use for now. Not exactly sure what "Load Page Buffer" does
void pr_load_page_buffer(uint8_t* buffer){
  printf("Load page buffer\n");
  uint8_t addr_buf[2] = {0,0};
  pr_run_command_wr(LOAD_PAGE_BUFFER, addr_buf, sizeof(addr_buf), buffer, 32);
}
