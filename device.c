#include "device.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

struct device at89lp213 = {
  .memsize = 2048,
  .pagesize = 32,
  .needs_prefix = 1,
  .signature = {0x1e, 0x27, 0xff},
  .commands = {
    {0b10101100}, 		        /* PROGRAM ENABLE */
    {0b10001010},			/* CHIP ERASE */
    {0b01100000},			/* READ STATUS */
    {0b01010001},			/* LOAD PAGE BUFFER */
    {0b01010000},			/* WRITE CODE PAGE */
    {0b01110000},			/* WRITE CODE PAGE ERASE */
    {0b00110000},			/* READ CODE PAGE */
    {0b11100001},			/* WRITE FUSES */
    {0b11110001},			/* WRITE FUSES ERASE */
    {0b01100001},			/* READ FUSES */
    {0b11100100},			/* WRITE LOCK */
    {0b01100100},			/* READ LOCK */
    {0b01010010},			/* WRITE USER SIGNATURE */
    {0b01110010},			/* WRITE SIGNATURE ERASE */
    {0b00110010},			/* READ USER SIGNATURE */
    {0b00111000},			/* READ ATMEL SIGNATURE */
  }
};

struct device at89s52 = {
  .memsize = 8192,
  .pagesize = 64,
  .needs_prefix = 0,
  .signature = {0x1e, 0x54, 0x06},
  .commands = {
    {0b10101100},		/* PROGRAM ENABLE */
    {0b10101100},		/* CHIP ERASE */
    {0b01100000},		/* READ STATUS */
    {0b00000000},		/* LOAD PAGE BUFFER (Unavailible) */
    {0b01010000},		/* WRITE CODE PAGE */
    {0b01110000},		/* WRITE CODE PAGE ERASE */
    {0b00110000},		/* READ CODE PAGE */
    {0b01010001},		/* WRITE FUSES */
    {0b01110001},		/* WRITE FUSES ERASE */
    {0b00110001},		/* READ FUSES */
    {0b01010100},		/* WRITE LOCK */
    {0b00110100},		/* READ LOCK */
    {0b01010010}, 		/* WRITE USER SIGNATURE */
    {0b01110010},		/* WRITE SIGNATURE ERASE */
    {0b00110010},		/* READ USER SIGNATURE */
    {0b00111000},		/* READ ATMEL SIGNATURE */
  }
};


struct device* device_from_string(char* str){
  if(strcasecmp(str,"at89lp213") == 0 || strcasecmp(str,"213") == 0){
    return &at89lp213;
  }
  else if(strcasecmp(str,"at89s52") == 0 || strcasecmp(str,"at89lp52") == 0 ||
	  strcasecmp(str,"52") == 0){
    return &at89s52;
  }
  else{
    fprintf(stderr, "Unknown device: %s\n",str);
    exit(1);
  }
}

int ilog2(int a){
  switch(a){
  case 1: return 0;
  case 2: return 1;
  case 4: return 2;
  case 8: return 3;
  case 16: return 4;
  case 32: return 5;
  case 64: return 6;
  case 128: return 7;
  case 256: return 8;
  default:
    fprintf(stderr,"ilog2: not a power of 2: %d\n", a);
    exit(1);

  }
  return -1;
}
