#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strarray.h"
#include "intarray.h"
#include "ssfp-client.h"
#include "parser.h"


struct ssfp_client_form {
  char *id;
  char *name;

  IntArray element_types;
  IntArray num_options;
  IntArray options_index;

  StrArray element_names;
  StrArray element_ids;
  StrArray element_texts;
  StrArray option_names;
  StrArray option_ids;

  int num_submits;
  StrArray submit_ids;
  StrArray submit_names;
};

struct ssfp_client_handle {
  char *context;
  char *session;
  int num_forms;
  SSFP_Form *forms;
  IntArray types;
  StrArray messages;
};

SSFP_Form SSFP_Form_create();
void SSFP_Form_destroy(SSFP_Form);
int parse_line(SSFP_Client client, Parser p);


SSFP_Client
SSFP_Client_create()
{
  SSFP_Client client = malloc(sizeof(struct ssfp_client_handle));
  client->context = NULL;
  client->session = NULL;
  client->num_forms = 0;
  client->forms = malloc(sizeof(SSFP_Form)*10);
  client->types = IntArray_create();
  client->messages = StrArray_create();
  return client;
}

int
SSFP_Client_parse_response(SSFP_Client client, char *str)
{
  Parser p = Parser_create(str);
  client->context = malloc(100);
  client->session = malloc(100);
  strcpy(client->context, Parser_line(p));
  Parser_nextline(p);
  strcpy(client->session, Parser_line(p));

  while (Parser_nextline(p)) {
    //if (parse_line(client, p) != 0) {
    //  return 1;
    //}
    parse_line(client, p);
  }
  return 0;
}

void
SSFP_Client_destroy(SSFP_Client client)
{
  free(client->context);
  free(client->session);
  for(int i = 0; i < client->num_forms; i++) {
    SSFP_Form_destroy(client->forms[i]);
  }
  IntArray_destroy(client->types);
  StrArray_destroy(client->messages);
  free(client);
}

const char*
SSFP_Client_session(SSFP_Client client)
{
  return client->session;
}

const char*
SSFP_Client_context(SSFP_Client client)
{
  return client->context;
}

int
SSFP_Client_num_directives(SSFP_Client client)
{
  return IntArray_length(client->types);
}

SSFP_Directive
SSFP_Client_get_directive(SSFP_Client client, int index)
{
  return IntArray_get(client->types, index);
}

const char*
SSFP_Client_get_status(SSFP_Client client, int status_index)
{
  return StrArray_get(client->messages, status_index);
}

SSFP_Form
SSFP_Client_get_form(SSFP_Client client, int form_index)
{
  if (form_index >= client->num_forms || form_index < 0) {
    return NULL;
  }
  return client->forms[form_index];
}

int
parse_line(SSFP_Client client, Parser p)
{
  char *type, *id, *name;
  StrArray option_ids, option_names;
  SSFP_Directive dir_type;
  SSFP_Element e_type;
  SSFP_Form form;

  switch (Parser_first_char(p)) {
  case '%':
    dir_type = SSFP_STATUS;
    break;
  case '&':
    dir_type = SSFP_FORM;
    break;
  default:
    dir_type = SSFP_ELEMENT;
    break;
  }

  if (dir_type == SSFP_STATUS) {
    IntArray_add(client->types, SSFP_STATUS);
    StrArray_add(client->messages, Parser_line(p)+1);
    return 0;
  }

  if (dir_type == SSFP_FORM) {
    form = SSFP_Form_create();
    client->forms[client->num_forms++] = form;
    IntArray_add(client->types, SSFP_FORM);
    form->id = Parser_field(p, 0, 0) + 1;
    form->name = Parser_field(p, 1, 1);
    return 0;
  }

  if (client->num_forms == 0) {
    return 1;
  }

  form = client->forms[client->num_forms - 1];

  if ((type = Parser_field(p,0,0)) == NULL) return 1;
  if ((id   = Parser_field(p,1,0)) == NULL) return 1;
  if ((name = Parser_field(p,2,1)) == NULL) return 1;

  if      (strcmp(type, "field")  == 0) e_type = SSFP_FIELD;
  else if (strcmp(type, "area")   == 0) e_type = SSFP_AREA;
  else if (strcmp(type, "radio")  == 0) e_type = SSFP_RADIO;
  else if (strcmp(type, "check")  == 0) e_type = SSFP_CHECK;
  else if (strcmp(type, "submit") == 0) e_type = SSFP_SUBMIT;
  else return 1;

  int num_options = -1;
  const char *element_texts = "";

  IntArray_add(form->options_index, StrArray_length(form->option_ids));
  
  switch(e_type) {
  case SSFP_SUBMIT:
    StrArray_add(form->submit_names, name);
    StrArray_add(form->submit_ids, id);
    break;
  case SSFP_FIELD:
  case SSFP_AREA:
    element_texts = Parser_data(p);
    break;
  case SSFP_RADIO:
  case SSFP_CHECK:
    IntArray_add(form->options_index, StrArray_length(form->option_ids));
    option_ids = Parser_option_ids(p);
    option_names = Parser_option_names(p);
    num_options = StrArray_length(option_ids);
    StrArray_add_arr(form->option_ids, option_ids);
    StrArray_add_arr(form->option_names, option_names);
    StrArray_destroy(option_ids);
    StrArray_destroy(option_names);
    break;
  default:
    return 1;
    break;
  }
  IntArray_add(form->element_types, e_type);
  StrArray_add(form->element_names, name);
  StrArray_add(form->element_ids, id);
  StrArray_add(form->element_texts, element_texts);
  IntArray_add(form->num_options, num_options);
  return 0;
}

SSFP_Form
SSFP_Form_create()
{
  SSFP_Form form = malloc(sizeof(struct ssfp_client_form));
  form->id = NULL;
  form->name = NULL;
  
  form->element_types = IntArray_create();
  form->num_options = IntArray_create();
  form->options_index = IntArray_create();

  form->element_names = StrArray_create();
  form->element_ids = StrArray_create();
  form->element_texts = StrArray_create();
  form->option_names = StrArray_create();
  form->option_ids = StrArray_create();

  form->num_submits = 0;
  form->submit_ids = StrArray_create();
  form->submit_names = StrArray_create();
  return form;
}

void
SSFP_Form_destroy(SSFP_Form form)
{
  free(form->id);
  free(form->name);
  IntArray_destroy(form->element_types);
  IntArray_destroy(form->num_options);
  IntArray_destroy(form->options_index);
  StrArray_destroy(form->element_names);
  StrArray_destroy(form->element_ids);
  StrArray_destroy(form->element_texts);
  StrArray_destroy(form->option_names);
  StrArray_destroy(form->option_ids);
  StrArray_destroy(form->submit_ids);
  StrArray_destroy(form->submit_names);
  free(form);
}


const char*
SSFP_Form_id(SSFP_Form form)
{
  return form->id;
}

const char*
SSFP_Form_name(SSFP_Form form)
{
  return form->name;
}

int
SSFP_Form_num_elements(SSFP_Form form)
{
  return IntArray_length(form->element_types);
}

SSFP_Element
SSFP_Form_element_type(SSFP_Form form, int element_index)
{
  return IntArray_get(form->element_types, element_index);
}

const char*
SSFP_Form_element_id(SSFP_Form form, int element_index)
{
  return StrArray_get(form->element_ids, element_index);
}

const char*
SSFP_Form_element_name(SSFP_Form form, int element_index)
{
  return StrArray_get(form->element_names, element_index);
}

const char*
SSFP_Form_element_default_text(SSFP_Form form, int element_index)
{
  if (element_index > StrArray_length(form->element_ids) || element_index < 0) {
    return NULL;
  }
  if (IntArray_get(form->num_options, element_index) != -1) {
    return NULL;
  }
  return StrArray_get(form->element_texts, element_index); 
}

StrArray
SSFP_Form_element_option_ids(SSFP_Form form, int element_index)
{
  int index, len;
  if (element_index > StrArray_length(form->element_ids) || element_index < 0) {
    return NULL;
  }
  if (IntArray_get(form->num_options, element_index) == -1) {
    return NULL;
  }
  StrArray arr = StrArray_create();
  len = IntArray_get(form->num_options, element_index);
  index = IntArray_get(form->options_index, element_index);
  for(int i = index; i < len+index; i++) {
    StrArray_add(arr, StrArray_get(form->option_ids, i));
  }
  return arr;
}

StrArray
SSFP_Form_element_option_names(SSFP_Form form, int element_index)
{
  int index, len;
  if (element_index > StrArray_length(form->element_ids) || element_index < 0) {
    return NULL;
  }
  if (IntArray_get(form->num_options, element_index) == -1) {
    return NULL;
  }
  StrArray arr = StrArray_create();
  len = IntArray_get(form->num_options, element_index);
  index = IntArray_get(form->options_index, element_index);
  for(int i = index; i < len+index; i++) {
    StrArray_add(arr, StrArray_get(form->option_names, i));
  }
  return arr;
}

void
SSFP_Client_request_start(SSFP_Form form, int form_index)
{
    
}

void
SSFP_Client_request_add_text(SSFP_Form form, int element_index, const char* str)
{
  
}

void
SSFP_Client_request_add_option(SSFP_Form form, int element_index, const char* option_id)
{
  
}

char*
SSFP_Client_request_generate(SSFP_Form formm)
{
  
}
