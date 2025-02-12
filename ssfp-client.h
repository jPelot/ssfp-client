#ifndef SSFP_CLIENT_H
#define SSFP_CLIENT_H

#include "strarray.h"

typedef enum SSFP_Element {
  SSFP_NONE,
  SSFP_FIELD,
  SSFP_AREA,
  SSFP_RADIO,
  SSFP_CHECK,
  SSFP_SUBMIT,
} SSFP_Element;

typedef enum SSFP_Directive {
  SSFP_FORM,
  SSFP_STATUS,
  SSFP_ELEMENT
} SSFP_Directive;

typedef struct ssfp_client_handle* SSFP_Client;
typedef struct ssfp_client_form* SSFP_Form;

SSFP_Client    SSFP_Client_create();
int            SSFP_Client_parse_response(SSFP_Client, char *str);
void           SSFP_Client_destroy(SSFP_Client);

const char*    SSFP_Client_session(SSFP_Client);
const char*    SSFP_Client_context(SSFP_Client);
int            SSFP_Client_num_directives(SSFP_Client);
SSFP_Directive SSFP_Client_get_directive(SSFP_Client, int index);
const char*    SSFP_Client_get_status(SSFP_Client, int status_index);
SSFP_Form      SSFP_Client_get_form(SSFP_Client, int form_index);

const char*  SSFP_Form_id(SSFP_Form);
const char*  SSFP_Form_name(SSFP_Form);
int          SSFP_Form_num_elements(SSFP_Form);
SSFP_Element SSFP_Form_element_type(SSFP_Form, int element_index);
const char*  SSFP_Form_element_id(SSFP_Form, int element_index);
const char*  SSFP_Form_element_name(SSFP_Form, int element_index);

const char*  SSFP_Form_element_default_text(SSFP_Form, int element_index);
int          SSFP_Form_element_num_options(SSFP_Form, int element_idnex);
StrArray     SSFP_Form_element_option_names(SSFP_Form, int element_index);
StrArray     SSFP_Form_element_option_ids(SSFP_Form, int element_index);

void SSFP_Client_request_start(SSFP_Form, int form_index);
void SSFP_Client_request_add_text(SSFP_Form, int element_index, const char* str);
void SSFP_Client_request_add_option(SSFP_Form, int element_index, const char* element_id);
char* SSFP_Client_request_generate(SSFP_Form);

#endif // SSFP_CLIENT_H
