#include <stdio.h>
#include "programmer.h"
int main(int argc, char** argv){
  pr_init();
  pr_enable_program_mode();
  uint8_t* signature =  pr_read_atmel_signature();
  free(signature);
  pr_read_user_fuses();
  //pr_write_user_fuses(default_fuses);
  pr_read_user_fuses();
  pr_destroy();
  return 0;
}
