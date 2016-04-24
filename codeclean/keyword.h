#ifndef _KEYWORD_H_
#define _KEYWORD_H_


#include "common.h"
#include "xstring.h"

extern void keyword_add(const char* name , int value);
extern int  keyword_lookup(const char* name );
extern void keyword_table_free();


#endif

