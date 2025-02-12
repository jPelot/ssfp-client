#include "strarray.h"

#ifndef PARSER_H
#define PARSER_H

typedef struct parser_handle_t* Parser;

Parser Parser_create(char *str);
void   Parser_destroy(Parser p);

const char *Parser_line(Parser p);
char       *Parser_field(Parser p, int index, int num);
const char *Parser_data(Parser p);
char        Parser_first_char(Parser p);
StrArray    Parser_option_ids(Parser p);
StrArray    Parser_option_names(Parser p);
int         Parser_nextline(Parser p);

#endif // PARSER_H
