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
  if((context = MPSSE(SPI0, ONE_MHZ/2, MSB)) == NULL || ! context->open){
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
  if(!options.device->needs_prefix)
    cmdbuf[0] = cmd;
  Start(context);
  print_buffer((uint8_t*) cmdbuf, options.device->needs_prefix ? sizeof(cmdbuf) : 1);
  Write(context, cmdbuf , options.device->needs_prefix ? sizeof(cmdbuf) : 1);
  if(addrsize){
    print_buffer(addrdata, addrsize);
    Write(context, (char*) addrdata, addrsize);
  }
  if(retbufsize){
    char* buf = Read(context, retbufsize);
    memcpy(retbuf,buf,retbufsize);
    print_buffer(retbuf, retbufsize);
    free(buf);
  }
  Stop(context);
 
}
void pr_run_command_wr(enum opcode cmd,
		       uint8_t* addrdata,
		       size_t addrsize,
		       uint8_t* writebuf,
		       size_t writebufsize){
  char cmdbuf[3] ={0xAA, 0x55, (char) cmd};
  if(!options.device->needs_prefix)
    cmdbuf[0] = cmd;
  Start(context);
  print_buffer((uint8_t*) cmdbuf, options.device->needs_prefix ? sizeof(cmdbuf) : 1);
  Write(context, cmdbuf, options.device->needs_prefix ? sizeof(cmdbuf) : 1);
  if(addrsize){
    print_buffer(addrdata, addrsize);
    Write(context, (char*) addrdata, addrsize);
  }
  if(writebufsize){
    print_buffer(writebuf, writebufsize);
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
  uint8_t buf[2] = {0b01010011,0};
  uint8_t ret = 0;
  pr_run_command_rd(PROGRAM_ENABLE, buf, sizeof(buf), &ret,1);
  printf("Program enable: 0x%x\n", ret);
  usleep(1000);
}
uint8_t* pr_read_atmel_signature(void){
  uint8_t buf[2] = {0,0};
  uint8_t ret[PAGE_SIZE_MAX] = {0};
  pr_run_command_rd(READ_ATMEL_SIGNATURE_PAGE, buf, sizeof(buf),ret,PAGE_SIZE);
  print_buffer(ret,PAGE_SIZE);
  uint8_t* ret_buf = malloc(PAGE_SIZE);
  memcpy(ret_buf,ret,PAGE_SIZE);
  return ret_buf;

}

uint8_t* pr_read_user_fuses(uint8_t* buffer){
  uint8_t buf[2] = {0,0};
  pr_run_command_rd(READ_USER_FUSES, buf, sizeof(buf), buffer, PAGE_SIZE);
  return buffer;
}

void pr_write_user_fuses(uint8_t* fuses){
  uint8_t buf[2] = {0,0};
  uint8_t data[PAGE_SIZE_MAX];
  memset(data,0xff, sizeof(data));
  memcpy(data,fuses,0x13);
  pr_run_command_wr(WRITE_USER_FUSES_ERASE, buf, sizeof(buf), data, PAGE_SIZE);
  usleep(7500);
}

void pr_chip_erase(void){
  pr_run_command_rd(CHIP_ERASE, NULL, 0, NULL, 0);
}

uint8_t pr_read_status(void){
  uint8_t addr[2] = {0,0};
  uint8_t status_ret;
  pr_run_command_rd(READ_STATUS, addr, sizeof(addr), &status_ret, sizeof(status_ret));
  return status_ret;
}

void pr_read_code_page(int pagenum, uint8_t* buffer){
  assert(pagenum < (options.device->memsize/PAGE_SIZE) && pagenum >= 0);
  uint8_t addr_buf[2];
  addr_buf[0] = (pagenum >> (8-ilog2(PAGE_SIZE))) & 0xff;
  addr_buf[1] = (pagenum << ilog2(PAGE_SIZE)) & 0xff;
  pr_run_command_rd(READ_CODE_PAGE, addr_buf, sizeof(addr_buf), buffer, PAGE_SIZE);
}

void pr_write_code_page(int pagenum, uint8_t* buffer){
  enum opcode op = options.auto_erase ? WRITE_CODE_PAGE_ERASE : WRITE_CODE_PAGE;
  printf("Write code page, %d bytes\n", PAGE_SIZE);
  assert(pagenum < (options.device->memsize/PAGE_SIZE) && pagenum >= 0);
  
  uint8_t addr_buf[2];
  addr_buf[0] = (pagenum >> (8-ilog2(PAGE_SIZE))) & 0xff;
  addr_buf[1] = (pagenum << ilog2(PAGE_SIZE)) & 0xff;
  if(buffer)
    pr_run_command_wr(op, addr_buf, sizeof(addr_buf), buffer, PAGE_SIZE);
  else
    pr_run_command_wr(op, addr_buf, sizeof(addr_buf), NULL, 0);
    usleep(5500); //5ms = write cycle time with auto-erase
}

//Don't use for now. Not exactly sure what "Load Page Buffer" does
void pr_load_page_buffer(uint8_t* buffer){
  printf("Load page buffer\n");
  uint8_t addr_buf[2] = {0,0};
  pr_run_command_wr(LOAD_PAGE_BUFFER, addr_buf, sizeof(addr_buf), buffer, PAGE_SIZE);
}

void pr_read_lock_bits(uint8_t* buffer){
  uint8_t addr[2] = {0,0};
  pr_run_command_rd(READ_LOCK_BITS, addr, sizeof(addr), buffer, PAGE_SIZE);
}
void pr_write_lock_bits(uint8_t* buffer){
  uint8_t addr[2] = {0,0};
  pr_run_command_wr(WRITE_LOCK_BITS, addr, sizeof(addr), buffer, PAGE_SIZE);
}
void pr_read_user_signature(uint8_t* buffer){
  uint8_t addr[2] = {0,0};
  pr_run_command_rd(READ_SIGNATURE_PAGE, addr, sizeof(addr), buffer, PAGE_SIZE);
  addr[1] = PAGE_SIZE;
  pr_run_command_rd(READ_SIGNATURE_PAGE, addr, sizeof(addr), buffer+PAGE_SIZE, PAGE_SIZE);
}
void pr_write_user_signature(uint8_t* buffer){
  uint8_t addr[2] = {0,0};
  pr_run_command_wr(WRITE_SIGNATURE_PAGE_ERASE, addr, sizeof(addr), buffer, PAGE_SIZE);
  addr[1] = PAGE_SIZE;
  pr_run_command_wr(WRITE_SIGNATURE_PAGE_ERASE, addr, sizeof(addr), buffer+PAGE_SIZE, PAGE_SIZE);
}
