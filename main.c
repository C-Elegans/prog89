#include <stdio.h>
#include <signal.h>
#include "programmer.h"

void cleanup(void){
  printf("Exiting\n");
  pr_destroy();
}
void inthandler(int dummy){
  exit(1);
}

int main(int argc, char** argv){
  pr_init();
  atexit(cleanup);
  signal(SIGINT, inthandler);
  pr_enable_program_mode();
  uint8_t code_buffer[32];
  uint8_t test_buffer[32];
  for(int i=0;i<32;i++){
    code_buffer[i] = i;
    test_buffer[i] = 32-i;
  }
  //pr_load_page_buffer(test_buffer);
  pr_write_code_page(0,code_buffer);
  
  pr_read_code_page(0,code_buffer);
  print_buffer(code_buffer,sizeof(code_buffer));
  return 0;
}
