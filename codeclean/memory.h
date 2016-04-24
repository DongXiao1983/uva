#ifndef _MEMORY_H_
#define _MEMORY_H_

typedef int error_opt;

enum error_types {
  FATAL = 1,
  WARNING = 2,
  ERROR = 3
};

#define xMalloc(n, type)        (type *)malloc((size_t)(n)* sizeof(type))
#define xCalloc(n, type)        (type *)calloc((size_t)(n), sizeof(type))
#define xRealloc(p, n , type)   (type *)realloc((p),(n) * sizeof(type))

extern void xfree (void *const ptr);

extern void error(const error_opt e_op, const char* format, ...);


#endif
