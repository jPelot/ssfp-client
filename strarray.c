#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "strarray.h"

#define INITIAL_SIZE 10

struct string_array_t {
  int strptr_allocated;
  int length;
  int index;
  char **arr;
};

int
has_print_char(const char *str) {
  while(str[0] != '\0') {
    if (isprint(str[0])) return 1;
    str = str+1;
  }
  return 0;
}

StrArray
StrArray_create()
{
  StrArray strarr = malloc(sizeof(struct string_array_t));
  strarr->arr = malloc(sizeof(char *) * INITIAL_SIZE);
  strarr->strptr_allocated = INITIAL_SIZE;
  strarr->length = 0;
  strarr->index  = 0;
  return strarr;
}

void
StrArray_destroy(StrArray arr)
{
  for(int i = 0; i < arr->length; i++) {
    free(arr->arr[i]);
  }
  free(arr->arr);
  free(arr);
}

void
StrArray_add(StrArray arr, const char *str)
{
  if (!has_print_char(str)) return;
  if (arr->length >= arr->strptr_allocated) {
    arr->strptr_allocated += INITIAL_SIZE;
    arr->arr = realloc(arr->arr, arr->strptr_allocated * sizeof(char *));
  }  
  int len = strlen(str);
  arr->arr[arr->length] = malloc(len+1);
  strcpy(arr->arr[arr->length], str);
  arr->length++;
}

const char*
StrArray_get(StrArray arr, int index)
{
  if (index < 0) {
    index = arr->length + index;
  }
  if (index < 0 || index >= arr->length) {
    return NULL;
  }
  return arr->arr[index];
}

const char*
StrArray_last(StrArray arr)
{
  if (arr->length == 0) {
    return NULL;
  }
  return arr->arr[arr->length-1];
}

void
StrArray_set(StrArray arr, int index, const char *str)
{
  if (index < 0 || index >=arr->length) {
    return;
  }
  free(arr->arr[index]);
  arr->arr[index] = malloc(strlen(str)+1);
  strcpy(arr->arr[index], str);
}

int
StrArray_length(StrArray arr)
{
  return arr->length;
}

void
StrArray_add_arr(StrArray arr, StrArray that)
{
  for(int i = 0; i < that->length; i++) {
    StrArray_add(arr, that->arr[i]);
  }
}

/*
void
StrArray_begining(StrArray arr)
{
  arr->index = 0;
}

const char*
StrArray_cur(StrArray arr) {
  if (arr->index >= arr->length) {
    return NULL;
  }
  return arr->arr[arr->index];
}

const char*
StrArray_next(StrArray arr) {
  const char* out = StrArray_cur(arr);
  if (out != NULL) arr->index++;
  return out;
}

int
StrArray_cmp(StrArray arr, const char *str) {
  const char *this;
  this = StrArray_cur(arr);
  if (this == NULL) return 0;
  return strcmp(this, str);
}
*/
char *
StrArray_combine(StrArray arr) {
  int total_length = 0;
  char *combined;
  char *p;
  for(int i = 0; i < arr->length; i++) {
    total_length += strlen(arr->arr[i]);
  }
  combined = malloc(total_length + 1);
  p = combined;
  for(int i = 0; i < arr->length; i++) {
    strcpy(p,arr->arr[i]);
    p += strlen(arr->arr[i]);
  }
  return combined;
}

char *
StrArray_splice(StrArray arr, const char* str) {
  int total_length = 0;
  char *combined;
  char *p;
  for(int i = 0; i < arr->length; i++) {
    total_length += strlen(arr->arr[i]);
  }
  combined = malloc(total_length + 1);
  p = combined;
  for(int i = 0; i < arr->length; i++) {
    strcpy(p,arr->arr[i]);
    p += strlen(arr->arr[i]);
  }
  return combined;
}
