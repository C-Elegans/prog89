#ifndef _IHEX_H
#define _IHEX_H
#include <stdint.h>
#include <stdio.h>
int parse_ihex(FILE* file, uint8_t** buffer, size_t* len);
uint32_t readhex(char*, int);
#endif
