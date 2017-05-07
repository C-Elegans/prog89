#ifndef _PROGRAMMER_H
#define _PROGRAMMER_H
#include <mpsse.h>
#include <stdint.h>
#include "writeopt.h"
enum opcode {
  PROGRAM_ENABLE		= 0b10101100,
  CHIP_ERASE			= 0b10001010,
  READ_STATUS			= 0b01100000,
  LOAD_PAGE_BUFFER		= 0b01010001,
  WRITE_CODE_PAGE		= 0b01010000,
  WRITE_CODE_PAGE_ERASE		= 0b01110000,
  READ_CODE_PAGE		= 0b00110000,
  WRITE_USER_FUSES		= 0b11100001,
  WRITE_USER_FUSES_ERASE	= 0b11110001,
  READ_USER_FUSES		= 0b01100001,
  WRITE_LOCK_BITS		= 0b11100100,
  READ_LOCK_BITS		= 0b01100100,
  WRITE_SIGNATURE_PAGE		= 0b01010010,
  WRITE_SIGNATURE_PAGE_ERASE	= 0b01110010,
  READ_SIGNATURE_PAGE		= 0b00110010,
  READ_ATMEL_SIGNATURE_PAGE	= 0b00111000,
};

#define PAGE_SIZE 32

void pr_init(void);

void		pr_destroy(void);

void		pr_run_command_rd(enum opcode cmd, uint8_t* addrdata, size_t addrsize, uint8_t* retbuf, size_t retbufsize);
void		pr_run_command_wr(enum opcode cmd, uint8_t* addrdata, size_t addrsize, uint8_t* writebuf, size_t writebufsize);

void		pr_enable_program_mode(void);
uint8_t*	pr_read_atmel_signature(void);
uint8_t*	pr_read_user_fuses(uint8_t* buffer);
void		pr_write_user_fuses(uint8_t* fuses);
void		pr_chip_erase(void);
uint8_t		pr_read_status(void);
void		pr_read_code_page(int pagenum, uint8_t* buffer);
void		pr_write_code_page(int pagenum, uint8_t* buffer);
void		pr_load_page_buffer(uint8_t* buffer);
		     

void print_buffer(uint8_t* buffer, size_t size);
#define MAX_WRITEOPTS 8
struct options{
  int chip_erase;
  int auto_erase;
  int nwriteopts;
  int memsize;
  struct writeopt* writeopts[MAX_WRITEOPTS];
};


extern struct options options;


extern uint8_t default_fuses[32];
#endif
