#include "common.h" 
#include <ctype.h>
#include <string.h>

#include <stdarg.h>
#include <errno.h>

#include "memory.h"

#define is_set(var, feature) (((int)(var) & (int)(feature)) == (int) feature)

extern void error(const error_opt e_op, const char* format, ...)
{
   va_list ap;
  
   va_start(ap, format);
   fprintf( stderr, "%s ", is_set(e_op, WARNING) ? "Warning: " :"");
   vfprintf( stderr, format, ap);

   if (is_set(e_op , ERROR))
      fprintf(stderr, " : %s", strerror(errno)); 

   fputs("\n",stderr);
   va_end(ap);

   if (is_set(e_op, FATAL))
      exit(1);
}


extern void *private_malloc( const size_t size)
{
  void *buffer = malloc(size);
  if ( NULL == buffer )
    error(FATAL, "out of memory");
  return buffer;
}

extern void *private_calloc (const size_t count, const size_t size)
{
   void *buffer = calloc (count, size);

	if (buffer == NULL)
		error (FATAL, "out of memory");

	return buffer;
}

extern void *private_realloc (void *const ptr, const size_t size)
{
	void *buffer;
	if (ptr == NULL)
		buffer = private_malloc (size);
	else
	{
		buffer = realloc (ptr, size);
		if (buffer == NULL)
			error (FATAL, "out of memory");
	}
	return buffer;
}

extern void xfree (void *const ptr)
{
	if (ptr != NULL) {
    	free (ptr);
   }
}
