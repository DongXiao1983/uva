#include "common.h"

#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file.h"
#include "memory.h"


/* DATA define */
input_file g_file;

char *g_current_directory;

static fpos_t g_start_of_line;


/* private function */


/* Return a newly-allocated string whose contents concatenate those of
 * s1, s2, s3.
 */
static char* __concat (const char *s1, const char *s2, const char *s3)
{
   int len1 = strlen (s1), len2 = strlen (s2), len3 = strlen (s3);
   char *result = xMalloc (len1 + len2 + len3 + 1, char);

   strcpy (result, s1);
   strcpy (result + len1, s2);
   strcpy (result + len1 + len2, s3);
   result [len1 + len2 + len3] = '\0';

   return result;
}


/*  Take for each encountered source newline.
 */

static void __file_newline (void)
{
	g_file.file_position  = g_start_of_line;
	g_file.new_line       = FALSE;
	g_file.line_number++;

}

static void __set_input_filename (const char *fileName)
{
	const char*  head = fileName;
	const char* tail = file_get_basename (head);

	if (g_file.name != NULL)
		string_delete(g_file.name);
    
	g_file.name = string_new_init(fileName);

	if (g_file.path != NULL)
		string_delete (g_file.path);
    
	if (tail == head){
		g_file.path = NULL;
    }
	else{
		const size_t length = tail - head - 1;
		g_file.path = string_new();
		string_ncopy(g_file.path, fileName, length);
	}
}

static int __file_get_ch()
{
   int   c;

   c = getc (g_file.fp);

   /* If previous character was a newline, then we're starting a line.    */

   if (g_file.new_line &&  c != EOF)
   {
      __file_newline();
      if (c == '#' )
      {
          fsetpos (g_file.fp, &g_start_of_line);
          c = getc (g_file.fp);
      }
   }

   if (c == EOF)
      g_file.eof = TRUE;
   else if (c == NEWLINE)
   {
      g_file.new_line = TRUE;
      fgetpos (g_file.fp, &g_start_of_line);
   }
   else if (c == CRETURN)
   {
      c = NEWLINE;  /* convert CR into newline */
      g_file.new_line = TRUE;
      fgetpos (g_file.fp, &g_start_of_line);
   }
   return c;
}


xString* __file_get_line()
{
   xString *result = NULL;
   int c;
   if (g_file.line == NULL)
      g_file.line = string_new();
   string_clear (g_file.line);
   do
   {
      c = __file_get_ch();
      if (c != EOF)
      string_set_char(g_file.line, c);
      if (c == '\n'  ||  (c == EOF  &&  get_string_length(g_file.line) > 0))
      {
         set_string_terminate(g_file.line);
         result = g_file.line;
         break;
      }
   } while (c != EOF);
   

   return result;

}


/* For caching of stat() calls */
extern fileStatus *file_stat (const char *const fileName)
{
	struct stat status;
	static fileStatus file;
	if (file.name == NULL  ||  strcmp (fileName, file.name) != 0)
	{
		file_stat_free (&file);
		file.name = strdup (fileName);
		if (lstat (file.name, &status) != 0)
			file.exists = FALSE;
		else
		{
			file.isSymbolicLink = (boolean) S_ISLNK (status.st_mode);
			if (file.isSymbolicLink  &&  stat (file.name, &status) != 0)
				file.exists = FALSE;
			else
			{
				file.exists = TRUE;
				file.isDirectory = (boolean) S_ISDIR (status.st_mode);
				file.isNormalFile = (boolean) (S_ISREG (status.st_mode));
				file.isExecutable = (boolean) ((status.st_mode &
					(S_IXUSR | S_IXGRP | S_IXOTH)) != 0);
				file.isSetuid = (boolean) ((status.st_mode & S_ISUID) != 0);
				file.size = status.st_size;
			}
		}
	}
	return &file;
}

extern void file_stat_free (fileStatus *status)
{
	if (status->name != NULL)
	{
		free(status->name);
		status->name = NULL;
	}
}


/*
 * Source file properties function
 */

extern const char* file_get_basename(const char*  filepath)
{

	const char *tail = strrchr (filepath, PATH_SEPARATOR);

	if (tail == NULL)
		tail = filepath;
	else
		++tail;  /* step past last delimiter */

   return tail;
}



extern int file_open(const char* filename)
{
   const char* mode = "r";
   int     ret  = FALSE;

   if ( g_file.fp != NULL ){
      fclose (g_file.fp);
      g_file.fp = NULL;
   }

   g_file.fp = fopen(filename,mode);

   if (g_file.fp == NULL){
      error (WARNING | ERROR, "cannot open \"%s\"", filename);
   }
   else {
      ret = TRUE;
      __set_input_filename(filename);
      fgetpos (g_file.fp, &g_start_of_line);
      fgetpos (g_file.fp, &g_file.file_position);
      g_file.current_line  = NULL;
      g_file.line_number   = 0L;
      g_file.eof           = FALSE;
      g_file.new_line      = TRUE;
      g_file.isHeader      = is_include_file (filename);
   }
   return ret;

}

extern void file_close(void)
{
   if( g_file.fp != NULL )
   {
      fclose (g_file.fp);
      g_file.fp = NULL; 
   }
}

extern int file_eof(void)
{
   return g_file.eof;
}


extern int file_skip2character (int c)
{
	int d;
	do
	{
		d = file_get_char();
	} while (d != EOF && d != c);
	return d;
}

extern void file_unget_char (int c)
{
	g_file.ungetch = c;
}


extern int file_get_char()
{
   int c;
   
   if ( g_file.ungetch != '\0' ){
      c = g_file.ungetch ;
      g_file.ungetch = '\0';
      return c;
   }

   do {
      if ( g_file.current_line != NULL ){
         c = *g_file.current_line++;
         if ( '\0' == c )
              g_file.current_line = NULL ;
      }
      else {
         xString *line = __file_get_line();
         if ( NULL != line )
            g_file.current_line = (unsigned char*) get_string_value (line);
         if ( g_file.current_line == NULL )
            c = EOF;
         else
            c = '\0';
      }
   }while( c == '\0');
   return c;
}


extern const unsigned char *file_get_line(void)
{
   xString* const line = __file_get_line();
   const unsigned char* result = NULL;
   if (line != NULL)
   {
      result = (const unsigned char*) get_string_value(line);
      string_strip_newline(line);
   }
   return result;
}


extern char* file_read_line(xString *const line , FILE* fp)
{
   char *result = NULL;
   
   string_clear(line);

   if ( NULL == fp )
         error(FATAL, "NULL file pointer");
   else{
      boolean re_readline;

      do {
         char *const pLastChar = get_string_value(line) + get_string_size (line) -2;
			fpos_t startOfLine;

			fgetpos (fp, &startOfLine);
			re_readline = FALSE;
			*pLastChar = '\0';
			result = fgets (get_string_value(line), (int) get_string_size(line), fp);
			if (result == NULL)
			{
				if (! feof(fp))
					error (FATAL | ERROR, "Failure on attempt to read file");
			}
			else if (*pLastChar != '\0'  &&
					 *pLastChar != '\n'  &&  *pLastChar != '\r')
			{
				/*  buffer overflow */
				re_readline = string_auto_resize(line);
				if (re_readline)
					fsetpos (fp, &startOfLine);
				else
					error (FATAL | ERROR, "input line too big; out of memory");
			}
			else
			{
				char* eol;
            line->length = strlen (line->buffer);

				/* canonicalize new line */
				eol = get_string_value(line) + get_string_length(line) - 1;
				if (*eol == '\r')
					*eol = '\n';
				else if (*(eol - 1) == '\r'  &&  *eol == '\n')
				{
					*(eol - 1) = '\n';
					*eol = '\0';
					--line->length;
				}
			}
      }while(re_readline);
   }
   return result;
}


extern const char *file_get_extension (const char *const fileName)
{
	const char *extension;
	const char *pDelimiter = NULL;
	const char *const base = file_get_basename (fileName);

	if (pDelimiter == NULL)
	    pDelimiter = strrchr (base, '.');

	if (pDelimiter == NULL)
		extension = "";
	else
		extension = pDelimiter + 1;  /* skip to first char of extension */

	return extension;
}



extern char* file_get_absolute_name (const char *file)
{
	char *slashp, *cp;
	char *res = NULL;
	if (is_absolute_path (file))
	{
		res = strdup (file);
	}
	else
		res = __concat (g_current_directory, file, "");

	/* Delete the "/dirname/.." and "/." substrings. */
	slashp = strchr (res, PATH_SEPARATOR);
	while (slashp != NULL  &&  slashp [0] != '\0')
	{
		if (slashp[1] == '.')
		{
			if (slashp [2] == '.' &&
				(slashp [3] == PATH_SEPARATOR || slashp [3] == '\0'))
			{
				cp = slashp;
				do
					cp--;
				while (cp >= res  &&  ! is_absolute_path(cp));
				if (cp < res)
					cp = slashp;/* the absolute name begins with "/.." */
				strcpy (cp, slashp + 3);
				slashp = cp;
				continue;
			}
			else if (slashp [2] == PATH_SEPARATOR  ||  slashp [2] == '\0')
			{
				strcpy (slashp, slashp + 2);
				continue;
			}
		}
		slashp = strchr (slashp + 1, PATH_SEPARATOR);
	}

	if (res [0] == '\0')
		return strdup ("/");
	else
	{
		return res;
	}
}

extern boolean is_include_file (const char *const fileName)
{
	boolean result = TRUE;
	const char *const extension = file_get_extension (fileName);
   result = strcasecmp(extension,"h");
	return !result;
}


extern boolean is_absolute_path (const char *const path)
{
	return path[0] == PATH_SEPARATOR ;
}


extern boolean is_same_file (const char *const name1, const char *const name2)
{
	boolean result = FALSE;
	struct stat stat1, stat2;

	if (stat (name1, &stat1) == 0  &&  stat (name2, &stat2) == 0)
		result = (boolean) (stat1.st_ino == stat2.st_ino);
   
	return result;
}

extern boolean is_recursive_link (const char* const dirName)
{
	boolean result = FALSE;
	fileStatus *status = file_stat (dirName);
	if (status->isSymbolicLink)
	{
		char* const path = file_get_absolute_name (dirName);
		while (path [strlen (path) - 1] == PATH_SEPARATOR)
			path [strlen (path) - 1] = '\0';
		while (! result  &&  strlen (path) > (size_t) 1)
		{
			char *const separator = strrchr (path, PATH_SEPARATOR);
			if (separator == NULL)
				break;
			else if (separator == path)  /* backed up to root directory */
				*(separator + 1) = '\0';
			else
				*separator = '\0';
			result = is_same_file (path, dirName);
		}
		xfree(path);
	}
	return result;
}

extern void set_current_directory (void)
{

	char* buf;

	if (g_current_directory == NULL)
		g_current_directory = xMalloc ((size_t) (PATH_MAX + 1), char);

	buf = getcwd (g_current_directory, PATH_MAX);
	if (buf == NULL)
		perror ("");

	if (g_current_directory [strlen (g_current_directory) - (size_t) 1] !=
			PATH_SEPARATOR)
	{
		sprintf (g_current_directory + strlen (g_current_directory), "%c",
				OUTPUT_PATH_SEPARATOR);
	}
}

extern xString *combine_path_and_file (const char *const path, const char *const file)
{
	xString *const filePath = string_new ();
	const int lastChar = path [strlen (path) - 1];
	boolean terminated = (lastChar ==  PATH_SEPARATOR);

	string_copy (filePath, path);
	if (! terminated)
	{
		string_set_char(filePath, OUTPUT_PATH_SEPARATOR);
		set_string_terminate (filePath);
	}
	string_cat (filePath, file);
	return filePath;
}

