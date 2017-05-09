#ifndef _PROGRAMMER_H
#define _PROGRAMMER_H
#include <mpsse.h>
#include <stdint.h>
#include "writeopt.h"
#include "device.h"
#include "command.h"

#define PAGE_SIZE options.device->pagesize
#define PAGE_SIZE_MAX 64

void pr_init(void);

void		pr_destroy(void);

void		pr_run_command_rd(enum opcode cmd, uint8_t* addrdata, size_t addrsize, uint8_t* retbuf, size_t retbufsize);
void		pr_run_command_wr(enum opcode cmd, uint8_t* addrdata, size_t addrsize, uint8_t* writebuf, size_t writebufsize);

void		pr_enable_program_mode(void);
uint8_t*	pr_read_atmel_signature(void);
uint8_t*	pr_read_user_fuses(uint8_t* buffer);
void		pr_write_user_fuses(uint8_t* fuses);
void            pr_read_lock_bits(uint8_t* buffer);
void            pr_write_lock_bits(uint8_t* buffer);
void            pr_read_user_signature(uint8_t* buffer);
void            pr_write_user_signature(uint8_t* buffer);
void		pr_chip_erase(void);
uint8_t		pr_read_status(void);
void		pr_read_code_page(int pagenum, uint8_t* buffer);
void		pr_write_code_page(int pagenum, uint8_t* buffer);
void		pr_load_page_buffer(uint8_t* buffer);
void            verify_signature(void);
		     

void print_buffer(uint8_t* buffer, size_t size);


#define MAX_WRITEOPTS 8
struct options{
  int chip_erase;
  int auto_erase;
  int nwriteopts;
  int pid;
  struct device* device;
  struct writeopt* writeopts[MAX_WRITEOPTS];
  char* config_file_name;
};


extern struct options options;


extern uint8_t default_fuses[32];
#endif
