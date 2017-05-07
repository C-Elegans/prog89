#include <stdio.h>
#include <signal.h>
#include <getopt.h>
#include "programmer.h"
#include "writeopt.h"
#include "run.h"

#define OPT_STRING "Dek:U:d:"

struct options options = {.auto_erase = 1, .chip_erase = 0, .device = &at89lp213};

void cleanup(void){
  printf("Exiting\n");
  pr_destroy();
}
void inthandler(int dummy){
  exit(1);
}

int main(int argc, char** argv){
  int c;
  while((c = getopt(argc, argv, OPT_STRING)) != -1){
    switch(c){
    case 'D':
      options.auto_erase = 0;
      break;
    case 'e':
      options.chip_erase = 1;
      break;
    case 'k':
      options.device->memsize = atoi(optarg);
      break;
    case 'U':{
      struct writeopt* wropt = decode_writeopt(optarg);
      if(options.nwriteopts < MAX_WRITEOPTS){
	options.writeopts[options.nwriteopts++] = wropt;
      } else {
	fprintf(stderr, "Too many write options (-U)\n");
	exit(1);
      }
      break;
    }
    case 'd':			/* Device */
      options.device = device_from_string(optarg);
      break;
    default:
      fprintf(stderr, "Unrecognized option -%c\n",c);
      exit(1);
    }
    
  }
  if(optind != argc){
    fprintf(stderr, "Non option argument: %s\n", argv[optind]);
    exit(1);
  }

  
  pr_init();
  atexit(cleanup);
  signal(SIGINT, inthandler);
  pr_enable_program_mode();
  run();
  return 0;
}
