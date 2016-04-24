#include <string.h>
#include <ctype.h>
#include <limits.h>   
#include "xstring.h"
#include "memory.h"

#define  STRING_INIT_SIZE 32

/*
 * Private function 
 */
void string_resize(xString *const string, const size_t new_size)
{

    char* new_buffer = NULL;
    new_buffer     = xRealloc(string->buffer, new_size, char);
    string->size   = new_size;
    string->buffer = new_buffer;
  
}


/* 
 * Public Interface 
 */

extern void string_delete(xString *const string)
{
    if ( NULL != string ) {
        if ( NULL != string->buffer ) {
              xfree(string->buffer);
        }
        xfree(string);
    }
}

extern void string_clear(xString *const string)
{
    string->length = 0;
    string->buffer[0]='\0';
}


extern xString *string_new(void)
{
    xString* string = xMalloc(1,xString);

    string->length = 0;
    string->size   = STRING_INIT_SIZE;
    string->buffer = xMalloc(string->size, char);
    string_clear(string);

    return string;
}


extern xString *string_new_init(const char* source)
{
    xString *string = string_new();
    string_cat(string, source);
    return string;
}


extern int string_auto_resize(xString *const string)
{
    int ret = 1;

    if ( string->size <= INT_MAX / 2 ){ 
        const size_t new_size =  string->size * 2;
        string_resize(string, new_size);
    }
    return ret;
}



extern void string_cat(xString *const string, const char*  source)
{
   size_t len = strlen(source);

   /* check string buffer overflow */
   while ( string->length + len + 1 > string->size ){
      string_auto_resize(string);
   }

   strcpy(string->buffer+string->length , source);

   string->length += len;
}



extern void string_ncat(xString *const string, const char* const source, const size_t length)
{
   const char *p = source;
   size_t remain = length;

   while( *p != '\0' && remain > 0 )
   {
      string_set_char(string, *p);
      remain--;
      p++;
   }

   set_string_terminate(string);
}



extern void string_ncopy(xString *const string, const char *const source, const size_t length)
{
	string_clear(string);
	string_ncat (string, source, length);

}


extern void string_copy (xString *const string, const char *const s)
{
	string_clear (string);
	string_cat (string, s);
}

extern void string_strip_newline (xString *const string)
{
	const size_t final = string->length - 1;
	if (string->buffer [final] == '\n')
	{
		string->buffer [final] = '\0';
		string->length--;
	}
}

extern void string_copy_to_lower (xString *const dest, const xString *const src)
{
	const size_t length = src->length;
	const char *s = src->buffer;
	char *d;
	size_t i;

	if (dest->size < src->size)
		string_resize (dest, src->size);
	d = dest->buffer;
	for (i = 0  ;  i < length  ;  ++i)
	{
		int c = s [i];

		d [i] = tolower (c);
	}
	d [i] = '\0';
}


extern boolean string_i_compare(const xString * src, const xString * dest)
{
   return (boolean) (strcasecmp (get_string_value(src), get_string_value (dest)) == 0);
}



extern boolean string_compare(const xString * src, const xString * dest)
{
   return (boolean) (strcmp (get_string_value(src), get_string_value (dest)) == 0);
}


extern xString* string_substr(const xString * string, int start, int end)
{
   xString *substr = string_new();
   if (start > end)
      string_ncopy(substr,string->buffer+start,0);
   else
      string_ncopy(substr,string->buffer+start,end-start);
   return substr;
}


extern boolean string_is_dig(const xString * string)
{
   int i = 0;
   const char *p = string->buffer;
   
   for ( i = 0 ; i < string->length ; i++ ){
      if (p[i]<'0' || p[i] > '9' )
         return FALSE;
   }
   
   return TRUE;
      
}


