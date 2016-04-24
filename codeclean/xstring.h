/* string struct to simulate the String Class.
 */

#ifndef _xstring_h_
#define _xstring_h_

#include "common.h"
#include <stdlib.h>

/* string define */

typedef struct string_st{
  size_t length;   /* buffer used */
  size_t size;     /* alloc size of buffer */
  char *buffer;    /* string */
}xString;


/* string properites operate */

#define string_set_char(s,c) \
	(void)(((s)->length + 1 == (s)->size ? string_auto_resize(s) : 0), \
	((s)->buffer [(s)->length] = (c)), \
	((c) == '\0' ? 0 : ((s)->buffer [++(s)->length] = '\0')))

#define string_value(xs)       ((xs)->buffer)
#define get_string_value(xs)   ((xs)->buffer)
#define get_string_char(xs,i)  ((xs)->buffer[i])
#define get_string_lastchar(xs) ((xs)->buffer[(xs)->length-1])
#define get_string_size(xs)     ((xs)->size)
#define get_string_length(xs)   ((xs)->length)


#define set_string_terminate(xs)  string_set_char(xs,'\0');

/* string operation */
extern xString *string_new(void);
extern xString *string_new_init(const char* string);

extern void string_clear(xString *const string);
extern void string_delete(xString *const string);
extern void string_cat(xString *const string, const char* const source);
extern void string_ncat(xString *const string, const char* const source, const size_t length);
extern void string_ncopy(xString *const string, const char *const source, const size_t length);

extern void string_strip_newline (xString *const string);
extern void string_copy (xString *const string, const char *const s);

extern int string_auto_resize(xString *const string);
extern void string_copy_to_lower (xString *const dest, const xString *const src);

extern boolean string_i_compare(const xString * src, const xString * dest);


extern boolean string_compare(const xString * src, const xString * dest);
extern xString* string_substr(const xString * string, int start, int end);

extern boolean string_is_dig(const xString * string);

#endif
