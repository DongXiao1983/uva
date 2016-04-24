#ifndef _FILE_H_
#define _FILE_H_


#include <ctype.h>
#include "xstring.h"

#define PATH_SEPARATOR '/'

#define OUTPUT_PATH_SEPARATOR	PATH_SEPARATOR



typedef struct input_file_st{

    FILE          *fp;

    xString       *name;                /* name of input file                         */

    xString       *path;                /* path of input file (if any)               */

    xString       *line;                /* last line read from file                  */

    unsigned char *current_line;        /* current processing line                  */

    unsigned long line_number;          /* line number in the input file           */

    fpos_t        file_position;        /* file position of current line             */

    int           eof;                  /* is the end of file?                         */

    int           new_line;             /* is next character begin a new line? */

    int           ungetch;              /* a single character that was ungotten */

    boolean       isHeader;             /* is head file */ 
}input_file;



typedef struct {
		
	char* name;              /* Name of file for which status is valid */
	 
	boolean exists;          /* Does file exist? If not, members below do not contain valid data. */
	
	boolean isSymbolicLink;  /* is file path a symbolic link to another file? */
		
	boolean isDirectory;     /* Is file (pointed to) a directory? */
		
	boolean isNormalFile;    /* Is file (pointed to) a normal file? */
		 
	boolean isExecutable;    /* Is file (pointed to) executable? */
		
	boolean isSetuid;        /* Is file (pointed to) setuid? */
		 
	unsigned long size;      /* Size of file (pointed to) */
} fileStatus; 


enum character{
   SPACE         = ' ',
   NEWLINE       = '\n',
   CRETURN       = '\r',
   FORMFEED      = '\f',
   TAB           = '\t',
   VTAB          = '\v',
   DOUBLE_QUOTE  = '"',
   SINGLE_QUOTE  = '\'',
   BACKSLASH     = '\\',

   STRING_SYMBOL = ('S' + 0x80),
   CHAR_SYMBOL   = ('C' + 0x80)
};



extern input_file g_file;


extern int  file_open(const char* file_name);
extern void file_close(void);
extern int file_eof(void);
extern int file_get_char();

extern const unsigned char *file_get_line(void);

extern const char *file_get_extension (const char *const fileName);

extern const char* file_get_basename(const char*  filepath);


extern void set_current_directory (void);

extern boolean is_include_file (const char *const fileName);

extern boolean is_absolute_path (const char *const path);

extern boolean is_recursive_link (const char* const dirName);

extern boolean is_same_file (const char *const name1, const char *const name2);

extern xString *combine_path_and_file (const char *const path, const char *const file);

extern fileStatus *file_stat (const char *const fileName);

extern void file_stat_free (fileStatus *status);

#endif
