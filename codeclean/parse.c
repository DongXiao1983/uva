#include "common.h"
#include <string.h>
#include "parse.h"
#include "file.h"
#include "chead.h"
#include "entry.h"
#include "xstring.h"
#include "memory.h"
#include "expression.h"

#define string_match(s1, s2)  (strcmp(s1,s2) == 0)




static current_state g_cstate = { '\0', '\0'};

typedef enum { COMMENT_NONE, COMMENT_C, COMMENT_CPLUS } comment;

static comment is_comment(void)
{
   comment cmnt;
   const int next = file_get_char();

   if ( next == '*')
        cmnt = COMMENT_C;
   else if ( next == '/' )
      cmnt = COMMENT_CPLUS;
   else{
         file_unget_char (next);
         cmnt = COMMENT_NONE;
   }
   return cmnt;
}

int skip_over_c_comment (void)
{
   int c = file_get_char();

   while (c != EOF)
   {
      if (c != '*')
         c = file_get_char ();
      else
      {
         const int next = file_get_char ();

         if (next != '/')
            c = next;
         else
         {
            c = SPACE;  /* replace comment with space */
            break;
         }
      }
   }
   return c;

}

static int  skip_over_whitespace(void)
{
   int c = file_get_char ();
   
   while (c == ' ' || c == '\t' ) {
      c =  file_get_char ();
   }
   return c;
}
/*  Skips over a C99 style comment.
 */
static int skip_over_cplus_comment (void)
{
	int c;

	while ((c = file_get_char ()) != EOF)
	{
		if (c == BACKSLASH)
			file_get_char ();  /* throw away next character, too */
		else if (c == NEWLINE)
			break;
	}
	return c;
}


static int skip2end_of_string (boolean ignoreBackslash)
{
	int c;

	while ((c = file_get_char ()) != EOF)
	{
		if (c == BACKSLASH && ! ignoreBackslash)
			file_get_char ();  /* throw away next character, too */
		else if (c == DOUBLE_QUOTE)
			break;
	}
	return STRING_SYMBOL;  /* symbolic representation of string */
}

static int skip2end_of_char (void)
{
	int c;
	int count = 0, veraBase = '\0';

	while ((c = file_get_char ()) != EOF)
	{
	    ++count;
		if (c == BACKSLASH)
			file_get_char ();  /* throw away next character, too */
		else if (c == SINGLE_QUOTE)
			break;
		else if (c == NEWLINE)
		{
			file_unget_char(c);
			break;
		}
		else if (count == 1  &&  strchr ("DHOB", toupper (c)) != NULL)
			veraBase = c;
		else if (veraBase != '\0'  &&  ! isalnum (c))
		{
			file_unget_char (c);
			break;
		}
	}
	return CHAR_SYMBOL;  
}


/* process Mutil Marco define such lisk #define AAA BBB+1 */
static boolean directive_has_pre_define(xString* value)
{
   int i = 0;
   for ( i = 0 ;i < value->length ; i++) {
      if(is_alph(value->buffer[i]))
         return TRUE;
   }
   return FALSE;
}


static void directive_get_define_value(xString* define_value, const xString *define_string)
{
   string_copy(define_value,define_string->buffer);
   entry_info_pre_marco(&define_value);   
}


static xString* directive_replace_define_value(xString* value)
{
   if ( ! directive_has_pre_define(value) )
      return value;

   int pos = 0 ;
   int def_pos = 0;
   int start = 0;
   
   xString *new_value     = string_new();
   xString *define_string = string_new();
   xString *define_value  = string_new();
   
   for ( pos = 0 ; pos < value->length ; pos++ ) {
      
      if(is_alph(value->buffer[pos])) {
 
         string_cat(new_value , string_substr(value,start, pos)->buffer);
         string_clear(define_string);
         
         for ( def_pos = pos ; def_pos < value->length && is_alph(value->buffer[def_pos]); def_pos++ ){
            string_set_char(define_string,(value->buffer[def_pos]));
         }
         
         directive_get_define_value(define_value,define_string);
         string_cat(new_value,define_value->buffer);
         
         start = def_pos;
         pos = def_pos-1;
      }     
   }
   string_cat(new_value , string_substr(value, start,value->length)->buffer);
   string_delete(define_string);
   string_delete(define_value);
   return new_value;
}



static void directive_set_define_value(entry_info *entry)
{
   xString *value = string_new ();
   
   int c;
   
   int result ;

   char buf[256] = {0};

   c = skip_over_whitespace();

	do
	{
		string_set_char (value, c);
	} while (c = file_get_char (), (c != EOF  && c !='\r' && c !='\n'));

   result = exp_eval(get_string_value( directive_replace_define_value(value)));

   sprintf (buf, "%d", result);

   string_cat(entry->value, buf);

   string_delete (value);
   

}

static void directive_make_define(xString* name)
{
   entry_info *entry = entry_new(name);

   entry->isroot = TRUE ;

   entry->type = string_new_init("macro");
   
   entry->ttagtype = TAG_DEFINE;
   
   directive_set_define_value(entry);
   
   entry_list_add(entry);

}

static void directive_read_Identifier (int c, xString *const name)
{
	string_clear (name);
	do
	{
		string_set_char (name, c);
	} while (c = file_get_char (), (c != EOF  &&  isident (c)));

	file_unget_char (c);
	set_string_terminate (name);
}


static void directive_define ( int c)
{
   xString *name= string_new();
   
   c = skip_over_whitespace();
   
	if (isident (c))
	{
		directive_read_Identifier (c, name);
		directive_make_define (name);
	}

   string_delete(name);
}


/*  Handles a pre-processor directive whose first character is given by "c".
 */
static boolean directive_process (int c)
{
	boolean ignore = FALSE;

	char directive [10] = {0};
    
    unsigned int i;
    
    for (i = 0  ;  i < 10 - 1  ;  ++i)
    {
        if (i > 0)
        {
            c = file_get_char();
            if (c == EOF  ||  ! isalpha (c))
            {
                g_file.ungetch = c;
                break;
            }
        }
        directive [i] = c;
    }
    directive [i] = '\0';  /* null terminate */

   if (string_match(directive,"define"))
      directive_define(c);
	return ignore;
}

extern void current_ungetc (const int c)
{
	g_cstate.ungetch2 = g_cstate.ungetch;
	g_cstate.ungetch = c;
}
extern int get_char(void)
{
    boolean directive  = FALSE;
    boolean ignore     = FALSE;
    boolean tag        = TRUE; 
    int c;
    if ( g_cstate.ungetch != '\0' ){
        c = g_cstate.ungetch;
        g_cstate.ungetch = g_cstate.ungetch2;
        g_cstate.ungetch2 = '\0';
        return c;
     }
    else do {
        c = file_get_char();

        switch(c){
            case EOF:
                ignore = FALSE;
                directive = FALSE;
                break;

            case TAB:
            case SPACE:
                break;

            case NEWLINE:
                if ( directive )
                    directive = FALSE;
                tag = TRUE;
                break;

            case DOUBLE_QUOTE:
                tag = FALSE;
                c = skip2end_of_string(FALSE);
                break;
            case SINGLE_QUOTE:
                tag = FALSE;
                c = skip2end_of_char ();
                break;

            case '#':
                if ( tag == TRUE ){
                    tag = FALSE;
                    directive = TRUE;
                }
                break;

            case '/':
            {
                const comment cmnt = is_comment();

                if (cmnt == COMMENT_C)
                    c = skip_over_c_comment ();
                else if (cmnt == COMMENT_CPLUS)
                {
                    c = skip_over_cplus_comment ();
                    if (c == NEWLINE)
                        file_unget_char (c);
                }
                else 
                    tag = FALSE;
                    break;
                }
            default:
                tag= FALSE;
                if (directive )
                    ignore = directive_process(c);
                break;
        }
    }while(directive || ignore);
    return c;
}



/*
*   File parsing
*/

extern boolean parse_file (const char *const fileName)
{
	if (file_open (fileName))
	{

      if ( g_file.isHeader ){
		    tag_finding();
      }
      
		file_close ();
      return TRUE;
	}
   return FALSE;
}

