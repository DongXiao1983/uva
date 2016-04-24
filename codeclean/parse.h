#ifndef _PARSE_H_
#define _PARSE_H_

#include "common.h"
#include "xstring.h"

/* The character valid as a character of a C identifier */
#define isident(c) (isalnum(c) || (c) == '_' || (c) == '$' )


typedef struct kind_option_st {
	boolean enabled;          /* are tags for kind enabled? */
	int letter;               /* kind letter */
	const char* name;         /* kind name */
	const char* description;  /* displayed in --help output */
} kind_option;


typedef struct current_state_st {
	int		ungetch;
    int      ungetch2;   /* ungotten characters, if any */
} current_state;


extern boolean parse_file (const char *const fileName);
extern int get_char(void);
extern void current_ungetc (const int c);

extern void file_unget_char (int c);

#endif

