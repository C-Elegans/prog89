#include "device.h"
#include "programmer.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <yaml.h>

static yaml_parser_t parser;
static FILE* config_file;
#define STACK_SIZE 32
yaml_event_t stack[STACK_SIZE];
int stack_pointer = 0;
yaml_event_t null_event = {
  .type = YAML_NO_EVENT,
};


static void init_parser(void){
  config_file = fopen(options.config_file_name, "r");
  if(!yaml_parser_initialize(&parser)){
    fprintf(stderr, "Failed to initialize parser\n");
    exit(1);
  }
  if(config_file == NULL){
    fprintf(stderr, "Failed to open file: %s\n", options.config_file_name);
    exit(1);
  }
  yaml_parser_set_input_file(&parser, config_file);
}

void push(yaml_event_t event){
  if(stack_pointer >= STACK_SIZE){
    fprintf(stderr,"Stack overflow\n"); exit(1);
  }
  stack[stack_pointer++] = event;
}
yaml_event_t pop(void){
  stack_pointer--;
  if(stack_pointer < 0){
    stack_pointer = 0;
    return null_event;
  }
  return stack[stack_pointer];
}

static int suffix_cmp(char* str1, char* str2){
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  if(len1 > len2){
    size_t difference = len1-len2;
    return strcmp(str1+difference, str2);
  } else {
    size_t difference = len2-len1;
    return strcmp(str1, str2+difference);
  }
}


static struct device* parse_file(char* device_name){
  struct device* cur_dev = NULL;
  yaml_event_t event;
  int command_pointer = 0;
  do{
    if(!yaml_parser_parse(&parser, &event)){
      fprintf(stderr, "Parse error in config file: %d\n", parser.error);
      exit(1);
    }
    switch(event.type){
    case YAML_NO_EVENT: break;
    case YAML_STREAM_START_EVENT: break;
    case YAML_STREAM_END_EVENT: break;
    case YAML_DOCUMENT_START_EVENT: break;
    case YAML_DOCUMENT_END_EVENT: break;
    case YAML_SEQUENCE_START_EVENT:
      //puts("Sequence start");
      push(event);
      command_pointer = 0;
      break;
    case YAML_SEQUENCE_END_EVENT: pop(); pop(); break;
    case YAML_MAPPING_START_EVENT:{
      yaml_event_t prev_event = pop();
      if(prev_event.type == YAML_SCALAR_EVENT){
	if(suffix_cmp((char*)prev_event.data.scalar.value, device_name) == 0){
	  cur_dev = malloc(sizeof(struct device));
	}
      }
      break;
    }
    case YAML_MAPPING_END_EVENT: break;
    case YAML_SCALAR_EVENT:{
      yaml_event_t prev_event = pop();
      //printf(" %s\n", event.data.scalar.value);
      if(prev_event.type == YAML_SCALAR_EVENT){
	char* key = (char*)prev_event.data.scalar.value;
	char* value = (char*)event.data.scalar.value;
	if(cur_dev){
	  if(strcmp(key, "memsize") == 0){
	    cur_dev->memsize = strtol(value, NULL, 0);
	  } else if(strcmp(key, "pagesize") == 0){
	    cur_dev->pagesize = strtol(value, NULL, 0);
	  } else if(strcmp(key, "needs_prefix") == 0){
	    cur_dev->needs_prefix = strtol(value, NULL, 0);
	  } else if(strcmp(key, "signature") == 0){
	    int sig = strtol(value, NULL, 0);
	    cur_dev->signature[0] = (sig>>16) & 255;
	    cur_dev->signature[1] = (sig>>8) & 255;
	    cur_dev->signature[2] = sig&255;
	    
	  } else if(strcmp(key,"uses_half_page") == 0){
	    cur_dev->uses_half_page = strtol(value, NULL, 0);
	  }
	  else {
	    fprintf(stderr, "Unrecognized Key: %s\n", key);
	    exit(1);
	  }
	}

      } else if(prev_event.type == YAML_SEQUENCE_START_EVENT){
	yaml_event_t prev_prev_event = pop();
	char* key = (char*) prev_prev_event.data.scalar.value;
	char* value = (char*)event.data.scalar.value;
	if(prev_prev_event.type == YAML_SCALAR_EVENT){
	  if(strcmp(key, "commands") == 0){
	    if(cur_dev){
	      if(command_pointer >= COMMAND_MAX){
		fprintf(stderr, "Too many command values!\n");
		exit(1);
	      }
	      cur_dev->commands[command_pointer].op = strtol(value, NULL, 2);
	      command_pointer++;
	    }
	  } else {
	    fprintf(stderr, "Unrecognized Key: %s\n", key);
	    exit(1);
	  }
	}

	push(prev_prev_event);
	push(prev_event);
      } else {
	push(event);
      }
      break;
    }
      
      
    default: puts("Unknown event"); break;

    }
    
  }while((event.type != YAML_STREAM_END_EVENT) && !(event.type == YAML_MAPPING_END_EVENT && cur_dev));
  yaml_event_delete(&event);
  yaml_parser_delete(&parser);
  fclose(config_file);
  return cur_dev;
}


struct device* device_from_string(char* str){
  init_parser();
  struct device* dev = parse_file(str);
  if(!dev){
    fprintf(stderr,"Could not find device\n");
    exit(1);
  }
  return dev;
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
