#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strarray.h"
#include "parser.h"


struct parser_handle_t {
  StrArray lines;
  int line_number;
  StrArray buffer;
};

Parser
Parser_create(char *str)
{
  Parser p;
  char *line, *index;
  int len;
  
  p = malloc(sizeof(struct parser_handle_t));
  p->lines = StrArray_create();
  p->buffer = StrArray_create();
  p->line_number = 0;
  line = str;
  do {
    index = strchr(line, '\r');
    if (index == NULL) {
      StrArray_add(p->lines, line);
      return p; 
    } else {
      *index = '\0';
      StrArray_add(p->lines, line);
      line = index + 1;
      if (*line == '\n') line++;
    }
  } while (index != NULL);
  return p;
}

void
Parser_destroy(Parser p)
{
  StrArray_destroy(p->lines);
  free(p);
}

const char*
Parser_line(Parser p)
{
  return StrArray_get(p->lines, p->line_number);
}

char*
Parser_field(Parser p, int index, int include_rest)
{
  char *temp, *ptr, *out, *base;
  const char *field;

  temp = malloc(strlen(Parser_line(p))+1);
  strcpy(temp, Parser_line(p));
  ptr = strchr(temp, '\n');
  if (ptr != NULL) {
    *ptr = '\0';
  }
  ptr = temp;
  for(int i = 0; i < index; i++) {
    ptr = strchr(ptr, ' ');
    if (ptr == NULL) {
      return NULL;
    }
    ptr++;
  }
  base = ptr;
  if (!include_rest) {
    ptr = strchr(base, ' ');
    if (ptr != NULL) {
      *ptr = '\0';
    }
  }
  out = malloc(strlen(base)+1);
  strcpy(out, base);
  return out;
}

const char*
Parser_data(Parser p)
{
  char *data = strchr(Parser_line(p), '\n');
  if (data == NULL) {
    return NULL;
  }
  return data+1;
}

char
Parser_first_char(Parser p)
{
  return Parser_line(p)[0];
}

StrArray
Parser_option_ids(Parser p)
{
  char *data, *option, *ptr;
  StrArray arr = StrArray_create();
  data = malloc(strlen(Parser_data(p))+1);
  strcpy(data, Parser_data(p));

  option = strtok(data, "\n");
  do {
    ptr = strchr(option, ' ');
    if (ptr != NULL) {
      *ptr = '\0';
    }
    StrArray_add(arr, ptr);
  } while ((option = strtok(NULL, "\n")) != NULL);

  free(data);
  return arr;  
}

StrArray
Parser_option_names(Parser p)
{
  char *data, *option, *ptr;
  StrArray arr = StrArray_create();
  data = malloc(strlen(Parser_data(p))+1);
  strcpy(data, Parser_data(p));

  option = strtok(data, "\n");
  do {
    ptr = strchr(option, ' ');
    if (ptr == NULL) {
      StrArray_add(arr, "");
    } else {
      StrArray_add(arr, ptr);
    }
  } while ((option = strtok(NULL, "\n")) != NULL);

  free(data);
  return arr;  
}


int
Parser_nextline(Parser p)
{
  if (p->line_number == StrArray_length(p->lines)-1) {
    return 0;
  }
  p->line_number++;
  return 1;
}


