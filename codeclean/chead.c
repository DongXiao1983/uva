#include "common.h"
#include <string.h>
#include <setjmp.h>
#include "file.h"
#include "parse.h"
#include "chead.h"
#include "memory.h"
#include "keyword.h"
#include "entry.h"




static jmp_buf Exception;
static xString *Signature;

static int AnonymousID = 0;

static statementInfo *CurrentStatement = NULL;

static boolean statement_is_contextual (const statementInfo *const st);

static void statement_reinit (statementInfo *const st, const boolean partial);

static void statement_process_name (statementInfo *const st);

static void statement_set_arraysize (statementInfo *const st);

static void token_parse_identifier (statementInfo *const st, const int c);




#define ckind_process_t

static kind_option g_ckinds[] = {
	{ TRUE,  'c', "class",      "classes"},
	{ TRUE,  'd', "macro",      "macro definitions"},
	{ TRUE,  'e', "enumerator", "enumerators (values inside an enumeration)"},
	{ TRUE,  'f', "function",   "function definitions"},
	{ TRUE,  'g', "enum",       "enumeration names"},
	{ FALSE, 'l', "local",      "local variables"},
	{ TRUE,  'm', "member",     "class, struct, and union members"},
	{ TRUE,  'n', "namespace",  "namespaces"},
	{ FALSE, 'p', "prototype",  "function prototypes"},
	{ TRUE,  's', "struct",     "structure names"},
	{ TRUE,  't', "typedef",    "typedefs"},
	{ TRUE,  'u', "union",      "union names"},
	{ TRUE,  'v', "variable",   "variable definitions"},
	{ FALSE, 'x', "externvar",  "external and forward variable declarations"}
};


static cKind ckind_get_by_tag (const tag_type type)
{
	cKind result = CK_UNDEFINED;
	switch (type)
	{
		case TAG_CLASS:      result = CK_CLASS;       break;
		case TAG_ENUM:       result = CK_ENUMERATION; break;
		case TAG_ENUMERATOR: result = CK_ENUMERATOR;  break;
		case TAG_FUNCTION:   result = CK_FUNCTION;    break;
		case TAG_LOCAL:      result = CK_LOCAL;       break;
		case TAG_MEMBER:     result = CK_MEMBER;      break;
		case TAG_NAMESPACE:  result = CK_NAMESPACE;   break;
		case TAG_PROTOTYPE:  result = CK_PROTOTYPE;   break;
		case TAG_STRUCT:     result = CK_STRUCT;      break;
		case TAG_TYPEDEF:    result = CK_TYPEDEF;     break;
		case TAG_UNION:      result = CK_UNION;       break;
		case TAG_VARIABLE:   result = CK_VARIABLE;    break;
		case TAG_EXTERN_VAR: result = CK_EXTERN_VARIABLE; break;

		default: break;
	}
	return result;
}

static boolean ckind_get_enabled (const tag_type type, const boolean isFileScope)
{
   boolean result;
   result = g_ckinds [ckind_get_by_tag (type)].enabled;
   return result;
}


static const char *ckind_get_name (const tag_type type)
{
	const char* result;

	result = g_ckinds [ckind_get_by_tag (type)].name;
   
	return result;
}


static void addContextSeparator (xString *const scope)
{
		string_cat (scope, "::");
}


extern boolean is_keyword(char *name, int *size)
{
   int i = 0 ; 
   while (Keyword_type_size[i].type){
      if ( strcmp(Keyword_type_size[i].type,name)==0){
         *size = Keyword_type_size[i].size;
         return TRUE;
      }
      i++;
   }
   return FALSE;
}


int get_type_size(char * type)
{
    int i = 0; 
    char *dec = NULL;
    for ( ; Keyword_type_size[i].type != NULL; i++){
        if (strcmp(type,Keyword_type_size[i].type) == 0)
            return Keyword_type_size[i].size;
    }
   
    dec = entry_list_query_item(type);
    for ( i =0 ; Keyword_type_size[i].type  && dec ; i++){
        if (strcmp(dec,Keyword_type_size[i].type) == 0)
            return Keyword_type_size[i].size;
    }
    
    return 0;
    
}

void set_entry_size(entry_info *entry, const statementInfo *const st)
{
    int array_size = 1;
    xString *as = string_new_init(string_value(st->arraysize));
    
    if ( st->isArray ) {
       if ( !string_is_dig(as)){
         entry_info_pre_marco(&(as));
       }
       array_size = strtol(string_value(as), NULL, 10)  ;
       array_size = array_size ? array_size:1;
       entry->isArray = TRUE; 
    }
    
    if ( entry->isPoint )
        entry->size = (array_size ? array_size : 1 )* sizeof(void *);
    else{
        entry->size = array_size * get_type_size(get_string_value(entry->type));
    }

    entry->array_count = array_size;
    string_delete(as);
}


#define skip_steps_t

/*  Skip to the next non-white character.
 */
static int skipToNonWhite (void)
{
	int c;

	while (1)
	{
		c = get_char ();
		if (!isspace (c))
			break;
	}

	return c;
}


/*  Skips to the next brace in column 1. This is intended for cases where
 *  preprocessor constructs result in unbalanced braces.
 */
static void skipToFormattedBraceMatch (void)
{
	int c, next;

	c = get_char ();
	next = get_char ();
	while (c != EOF  &&  (c != '\n'  ||  next != '}'))
	{
		c = next;
		next = get_char ();
	}
}


static void skipToMatch (const char *const pair)
{
	const boolean braceMatching = (boolean) (strcmp ("{}", pair) == 0);
	const boolean braceFormatting = (boolean) ( braceMatching);
	const int begin = pair [0], end = pair [1];
	const unsigned long inputLineNumber = g_file.line_number;
	int matchLevel = 1;
	int c = '\0';

	while (matchLevel > 0  &&  (c = skipToNonWhite ()) != EOF)
	{
		if (c == begin)
		{
			++matchLevel;
			if (braceFormatting )
			{
				skipToFormattedBraceMatch ();
				break;
			}
		}
		else if (c == end)
		{
			--matchLevel;
			if (braceFormatting  )
			{
				skipToFormattedBraceMatch ();
				break;
			}
		}
	}
	if (c == EOF)
	{
		printf("%s: failed to find match for '%c' at line %lu\n",
				get_string_value(g_file.name), begin, inputLineNumber);
	}
}

static void skipParens (void)
{
	const int c = skipToNonWhite ();

	if (c == '(')
		skipToMatch ("()");
	else
		current_ungetc (c);
}

static void skipBraces (void)
{
	const int c = skipToNonWhite ();

	if (c == '{')
		skipToMatch ("{}");
	else
		current_ungetc (c);
}


#define token_process_t

static void token_init (token_info* const token)
{
   token->type       = TOKEN_NONE;
   token->keyword    = KEYWORD_NONE;
   string_clear (token->name);
}

static void token_set (statementInfo *const st, const token_type type)
{
	token_info *token;
	token = activeToken (st);
	token_init (token);
	token->type = type;
}




static token_info *token_prev (const statementInfo *const st, unsigned int n)
{
	unsigned int tokenIndex;
	unsigned int num = (unsigned int) NumTokens;
//	Assert (n < num);
	tokenIndex = (st->tokenIndex + num - n) % num;
	return st->token [tokenIndex];
}



static void token_parse_general (statementInfo *const st, const int c)
{
	const token_info *const prev = token_prev (st, 1);
	
	if (isident (c) )
	{
		token_parse_identifier (st, c);
		if (isType (st->context, TOKEN_NAME) &&
			isType (activeToken (st), TOKEN_NAME) && isType (prev, TOKEN_NAME))
		{
			token_init (st->context);
		}
	}
	else if (c == '.' || c == '-')
	{
		if (! st->assignment)
			st->notVariable = TRUE;
		if (c == '-')
		{
			int c2 = get_char ();
			if (c2 != '>')
				current_ungetc (c2);
		}
	}
	else if (c == '!' || c == '>')
	{
		int c2 = get_char ();
		if (c2 != '=')
			current_ungetc (c2);
	}
	else if (isExternCDecl (st, c))
	{
		st->scope = SCOPE_GLOBAL;
	}
}

static void token_next (statementInfo *const st)
{
	token_info *token;
   
	do
	{
		int c = skipToNonWhite ();
		switch (c)
		{
			case EOF: longjmp (Exception, (int) ExceptionEOF);            break;
			//case '(':                        break;
			//case '<':                        break;
			case '*': st->haveQualifyingName = FALSE;st->isPointer= TRUE; break;
			case ',': token_set (st, TOKEN_COMMA);                        break;
			//case ':':                        break;
			case ';': token_set (st, TOKEN_SEMICOLON);                    break;
			//case '=':                        break;
			case '[': statement_set_arraysize(st);skipToMatch ("[]");     break;
			case '{': token_set (st, TOKEN_BRACE_OPEN);                   break;
			case '}': token_set (st, TOKEN_BRACE_CLOSE);                  break;
			default:  token_parse_general (st, c);                        break;
		}
		token = activeToken (st);
	} while (isType (token, TOKEN_NONE));
}

static void token_copy (token_info *const dest, const token_info *const src)
{
	dest->type         = src->type;
	dest->keyword      = src->keyword;
	dest->filePosition = src->filePosition;
	dest->lineNumber   = src->lineNumber;
	string_copy (dest->name, get_string_value(src->name));
}

static void token_advance (statementInfo* const st)
{
	if (st->tokenIndex >= (unsigned int) NumTokens - 1)
		st->tokenIndex = 0;
	else
		++st->tokenIndex;
	token_init(st->token [st->tokenIndex]);
}


static token_info *token_new (void)
{
	token_info *const token = xMalloc (1, token_info);
	token->name = string_new ();
	token_init (token);
	return token;
}

static void token_delete (token_info *const token)
{
	if (token != NULL)
	{
		string_delete (token->name);
		xfree(token);
	}
}

/*
*   Token parsing functions
*/

static void token_add_context (statementInfo *const st, const token_info* const token)
{
	if (isType (token, TOKEN_NAME))
	{
		if (get_string_length(st->context->name) > 0)
		{
			
				string_cat (st->context->name, "::");

		}
		string_cat (st->context->name, get_string_value(token->name));
		st->context->type = TOKEN_NAME;
	}
}

static keywordId token_analyze_keyword (const char *const name)
{
	const keywordId id = (keywordId) keyword_lookup (name);
	return id;
}




static void token_process (token_info *const token, statementInfo *const st)
{
	switch (token->keyword)        /* is it a reserved word? */
	{
		default: break;

		case KEYWORD_NONE:      statement_process_name (st);            break;
		case KEYWORD_ATTRIBUTE: skipParens (); token_init (token);      break;
		case KEYWORD_BIND:      st->declaration = DECL_BASE;            break;
		case KEYWORD_BIT:       st->declaration = DECL_BASE;            break;
		case KEYWORD_CATCH:     skipParens (); skipBraces ();           break;
		case KEYWORD_CHAR:      st->declaration = DECL_BASE;            break;
		case KEYWORD_CLASS:     st->declaration = DECL_CLASS;           break;
		case KEYWORD_CONST:     st->declaration = DECL_BASE;            break;
		case KEYWORD_DOUBLE:    st->declaration = DECL_BASE;            break;
		case KEYWORD_ENUM:      st->declaration = DECL_ENUM;            break;
		case KEYWORD_FLOAT:     st->declaration = DECL_BASE;            break;
		case KEYWORD_FUNCTION:  st->declaration = DECL_BASE;            break;
		case KEYWORD_FRIEND:    st->scope       = SCOPE_FRIEND;         break;
		case KEYWORD_INT:       st->declaration = DECL_BASE;            break;
		case KEYWORD_INTEGER:   st->declaration = DECL_BASE;            break;
		case KEYWORD_LONG:      st->declaration = DECL_BASE;            break;
      case KEYWORD_SHORT:     st->declaration = DECL_BASE;            break;
		case KEYWORD_SIGNED:    st->declaration = DECL_BASE;            break;
		case KEYWORD_STRING:    st->declaration = DECL_BASE;            break;
		case KEYWORD_STRUCT:    st->declaration = DECL_STRUCT;          break;
		case KEYWORD_UNION:     st->declaration = DECL_UNION;           break;
		case KEYWORD_UNSIGNED:  st->declaration = DECL_BASE;            break;
		case KEYWORD_VOID:      st->declaration = DECL_BASE;            break;
		case KEYWORD_VOLATILE:  st->declaration = DECL_BASE;            break;
		case KEYWORD_WCHAR_T:   st->declaration = DECL_BASE;            break;
		
		case KEYWORD_EVENT:
			break;

		case KEYWORD_TYPEDEF:
			statement_reinit (st, FALSE);
			st->scope = SCOPE_TYPEDEF;
			break;

		case KEYWORD_EXTERN:
			if ( !st->gotName)
			{
				statement_reinit (st, FALSE);
				st->scope = SCOPE_EXTERN;
				st->declaration = DECL_BASE;
			}
			break;

		case KEYWORD_STATIC:
				statement_reinit (st, FALSE);
				st->scope = SCOPE_STATIC;
				st->declaration = DECL_BASE;
			break;

		case KEYWORD_FOR:
		case KEYWORD_FOREACH:
		case KEYWORD_IF:
		case KEYWORD_SWITCH:
		case KEYWORD_WHILE:
		{
			int c = skipToNonWhite ();
			if (c == '(')
				skipToMatch ("()");
			break;
		}
	}
}


static void token_analyze_identifier (token_info *const token)
{
	char *const name = get_string_value(token->name);


    token->keyword = token_analyze_keyword (name);

    if (token->keyword == KEYWORD_NONE)
        token->type = TOKEN_NAME;
    else
        token->type = TOKEN_KEYWORD;


}

static void token_read_identifier (token_info *const token, const int firstChar)
{
	xString *const name = token->name;
	int c = firstChar;

	token_init (token);

	do
	{
		string_set_char (name, c);

		c = get_char ();
	} while (isident (c) );
	set_string_terminate(name);
	current_ungetc (c);        /* unget non-identifier character */

	token_analyze_identifier (token);
}




static void token_parse_identifier (statementInfo *const st, const int c)
{
	token_info *const token = activeToken (st);

	token_read_identifier (token, c);
	if (! isType (token, TOKEN_NONE))
		token_process (token, st);
}


static boolean token_is_contextual_keyword (const token_info *const token)
{
	boolean result;
	switch (token->keyword)
	{
		case KEYWORD_CLASS:
		case KEYWORD_ENUM:
		case KEYWORD_INTERFACE:
		case KEYWORD_NAMESPACE:
		case KEYWORD_STRUCT:
		case KEYWORD_UNION:
			result = TRUE;
			break;

		default: result = FALSE; break;
	}
	return result;
}


#define statement_process_t


static void statement_init_memberinfo (statementInfo *const st)
{
	access_type accessDefault = ACCESS_UNDEFINED;

	if (st->parent != NULL) switch (st->parent->declaration)
	{
		case DECL_ENUM:
			accessDefault =  ACCESS_UNDEFINED;
			break;


		case DECL_CLASS:
			accessDefault = ACCESS_PRIVATE;
            
		case DECL_STRUCT:
		case DECL_UNION:
			accessDefault = ACCESS_PUBLIC;
			break;

		default: break;
	}
	st->member.accessDefault = accessDefault;
	st->member.access		 = accessDefault;
}

static void statement_init (statementInfo *const st, statementInfo *const parent)
{
	st->parent = parent;
	statement_init_memberinfo (st);
	statement_reinit (st, FALSE);
}

static statementInfo *statement_new (statementInfo *const parent)
{
	statementInfo *const st = xMalloc (1, statementInfo);
	unsigned int i;

	for (i = 0  ;  i < (unsigned int) NumTokens  ;  ++i)
		st->token [i] = token_new ();

	st->context = token_new ();
	st->blockName = token_new ();
	st->parentClasses = string_new ();

	statement_init (st, parent);
	CurrentStatement = st;

	return st;
}


static void statement_reinit (statementInfo *const st, const boolean partial)
{
	unsigned int i;

	if (! partial)
	{
		st->scope = SCOPE_GLOBAL;
		if (statement_is_contextual (st->parent))
			st->declaration = DECL_BASE;
		else
			st->declaration = DECL_NONE;
	}
   
	st->gotParenName	     = FALSE;
	st->isPointer		     = FALSE;
	st->inFunction		     = FALSE;
	st->assignment		     = FALSE;
	st->notVariable	     = FALSE;
	st->gotArgs			     = FALSE;
	st->gotName			     = FALSE;
	st->haveQualifyingName = FALSE;
	st->tokenIndex		     = 0;
   st->isArray            = FALSE;
   st->arraysize          = string_new();
   
	if (st->parent != NULL)
		st->inFunction = st->parent->inFunction;

	for (i = 0  ;  i < (unsigned int) NumTokens  ;  ++i)
		token_init (st->token [i]);

	token_init (st->context);

	/*	Keep the block name, so that a variable following after a comma will
	 *	still have the structure name.
	 */
	if (! partial)
		token_init (st->blockName);

	string_clear (st->parentClasses);

	/*  Init member info.
	 */
	if (! partial)
		st->member.access = st->member.accessDefault;
}


static void statement_delete (void)
{
	statementInfo *const st = CurrentStatement;
	statementInfo *const parent = st->parent;
	unsigned int i;

	for (i = 0  ;  i < (unsigned int) NumTokens  ;  ++i)
	{
		token_delete (st->token [i]);       st->token [i]     = NULL;
	}
   
	token_delete (st->blockName);          st->blockName     = NULL;
	token_delete (st->context);            st->context       = NULL;
	string_delete (st->parentClasses);     st->parentClasses = NULL;
   
	xfree (st);
   
	CurrentStatement = parent;
}

static void statement_delete_all(void)
{
	while (CurrentStatement != NULL)
		statement_delete ();
}

static void statement_process_name (statementInfo *const st)
{

	if (st->gotName  &&  st->declaration == DECL_NONE)
		st->declaration = DECL_BASE;
	st->gotName = TRUE;
	st->haveQualifyingName = TRUE;
}

static boolean statement_is_contextual (const statementInfo *const st)
{
	boolean result = FALSE;
	if (st != NULL) switch (st->declaration)
	{
		case DECL_CLASS:
		case DECL_ENUM:
		case DECL_STRUCT:
		case DECL_UNION:
			result = TRUE;
			break;

		default: result = FALSE; break;
	}
	return result;
}

static boolean statement_is_end (const statementInfo *const st)
{
	const token_info *const token = activeToken (st);
	boolean isEnd;

	if (isType (token, TOKEN_SEMICOLON))
		isEnd = TRUE;
	else if (isType (token, TOKEN_BRACE_CLOSE))
		isEnd = (boolean) (!statement_is_contextual (st));
	else
		isEnd = FALSE;

	return isEnd;
}

static void statement_set_arraysize (statementInfo *const st)
{
    char *h = (char *)g_file.current_line;
    char *p = strchr( h,']');
    
    if ( p != NULL) {
        string_ncopy(st->arraysize,(char *)g_file.current_line,p-h);
        st->isArray = TRUE;
    }
}


static void statement_check_end (statementInfo *const st)
{
	const token_info *const token = activeToken (st);

	if (isType (token, TOKEN_COMMA))
		statement_reinit (st, TRUE);
	else if (statement_is_end (st))
	{
		statement_reinit (st, FALSE);
	}
	else
	{
	   token_advance (st);
	}
}

static boolean statement_check_member (const statementInfo *const st)
{
	boolean result;
	if (isType (st->context, TOKEN_NAME))
		result = TRUE;
	else
		result = (boolean)
			(st->parent != NULL && statement_is_contextual (st->parent));
	return result;
}


static void statement_find_scope_hierarchy (xString *const string,
								const statementInfo *const st)
{
	string_clear (string);
	if (isType (st->context, TOKEN_NAME))
		string_copy (string, st->context->name->buffer);
	if (st->parent != NULL)
	{
		xString *temp = string_new ();
		const statementInfo *s;
		for (s = st->parent  ;  s != NULL  ;  s = s->parent)
		{
			if (statement_is_contextual (s))
			{
				string_copy (temp, string->buffer);
				string_clear (string);
				if (isType (s->context, TOKEN_NAME) && (s->context->name->length) > 0)
				{
					string_cat (string, s->context->name->buffer);
               addContextSeparator (string);
				}
				string_cat (string, s->blockName->name->buffer);
				if ((temp->length) > 0)
					addContextSeparator (string);

				string_cat (string, temp->buffer);
			}
		}
		string_delete (temp);
	}
}



#define tag_process 


static tag_type tag_get_type_by_decl (const decl_type declaration)
{
	tag_type type = TAG_UNDEFINED;

	switch (declaration)
	{
		case DECL_CLASS:        type = TAG_CLASS;       break;
		case DECL_ENUM:         type = TAG_ENUM;        break;
		case DECL_EVENT:        type = TAG_EVENT;       break;
		case DECL_FUNCTION:     type = TAG_FUNCTION;    break;
		case DECL_STRUCT:       type = TAG_STRUCT;      break;
		case DECL_UNION:        type = TAG_UNION;       break;

		default: break;
	}
	return type;
}


static void tag_add_other_fields (entry_info* const et, const tag_type type,
							const statementInfo *const st, xString *const scope , xString *const typeRef)
{
	/*  For selected tag types, append an extension flag designating the
	 *  parent object in which the tag is defined.
	 */

	/* Add typename info, type of the tag and name of struct/union/etc. */
	if ((type == TAG_TYPEDEF || type == TAG_VARIABLE || type == TAG_MEMBER)
			&& statement_is_contextual(st))
	{
		char *p;

		string_copy(et->typeRef [0] ,ckind_get_name(tag_get_type_by_decl (st->declaration)));
		p = get_string_value (st->blockName->name);

		/*  If there was no {} block get the name from the token before the
		 *  name (current token is ';' or ',', previous token is the name).
		 */
		if (p == NULL || *p == '\0')
		{
			token_info * prev2 = token_prev (st, 2);
			if (isType (prev2, TOKEN_NAME))
				p = get_string_value (prev2->name);
		}

		/* Prepend the scope name if there is one. */
		if ((scope->length) > 0)
		{
			string_copy(typeRef, scope->buffer);
			addContextSeparator (typeRef);
			string_cat(typeRef, p);
			p = get_string_value (typeRef);

		}
		string_copy(et->typeRef [1], p);
	}
}



static void tag_make (const token_info *const token, const statementInfo *const st,
					 boolean isFileScope, const tag_type type)
{


   entry_info *entry = entry_new(token->name);

   entry->isroot = ( st->parent == NULL ) ? 1 : 0;

   token_info * prev1 = token_prev (st, 2);
   string_cat(entry->type, prev1->name->buffer);


   if ( st->isPointer ) {
      string_cat(entry->type,"*");
      entry->isPoint = 1;
   }
   
   set_entry_size(entry,st);
   xString *typeRef = string_new ();
   xString *scope = string_new ();
   entry->ttagtype = type;
   entry->keyword  = keyword_lookup(prev1->name->buffer);

	statement_find_scope_hierarchy (scope, st);
   tag_add_other_fields(entry,  type,st, scope, typeRef);
    

   entry_list_add(entry);

   string_delete (scope);
   string_delete (typeRef);

}


static void tag_qualify_enumerator (const statementInfo *const st,
								  const token_info *const nameToken)
{
	if (isType (nameToken, TOKEN_NAME))
		tag_make (nameToken, st, TRUE, TAG_ENUMERATOR);
}



static void tag_qualify_function (const statementInfo *const st,
								const token_info *const nameToken)
{
	if (isType (nameToken, TOKEN_NAME))
	{
		tag_type type;
		const boolean isFileScope =
						(boolean) (st->member.access == ACCESS_PRIVATE ||
						(!statement_check_member(st)  &&  st->scope == SCOPE_STATIC));
			type = TAG_FUNCTION;
		tag_make (nameToken, st, isFileScope, type);
	}
}

static boolean tag_valid_type_specifier (const decl_type declaration)
{
	boolean result;
	switch (declaration)
	{
		case DECL_BASE:
		case DECL_CLASS:
		case DECL_ENUM:
		case DECL_EVENT:
		case DECL_STRUCT:
		case DECL_UNION:
			result = TRUE;
			break;

		default:
			result = FALSE;
			break;
	}
	return result;
}

static void tag_qualify_function_decl(const statementInfo *const st,
									const token_info *const nameToken)
{
	if (! isType (nameToken, TOKEN_NAME))
		;
	else if (st->scope == SCOPE_TYPEDEF)
		tag_make (nameToken, st, TRUE, TAG_TYPEDEF);
	else if (tag_valid_type_specifier (st->declaration))
		tag_make (nameToken, st, TRUE, TAG_PROTOTYPE);
}

static void tag_qualify_compound(const statementInfo *const st,
								const token_info *const nameToken)
{
	if (isType (nameToken, TOKEN_NAME))
	{
		const tag_type type = tag_get_type_by_decl (st->declaration);
		const boolean fileScoped = FALSE;

		if (type != TAG_UNDEFINED)
			tag_make (nameToken, st, fileScoped, type);
	}
}

static void tag_qualify_block (statementInfo *const st,
							 const token_info *const nameToken)
{
	switch (st->declaration)
	{
		case DECL_CLASS:
		case DECL_ENUM:
		case DECL_STRUCT:
		case DECL_UNION:
			tag_qualify_compound (st, nameToken);
			break;
		default: break;
	}
}


static void tag_qualify_variable(const statementInfo *const st,	const token_info *const nameToken)
{
	if (! isType (nameToken, TOKEN_NAME))
		;
	else if (st->scope == SCOPE_TYPEDEF)
		tag_make (nameToken, st, TRUE, TAG_TYPEDEF);
	else if (st->declaration == DECL_EVENT)
		tag_make (nameToken, st, (boolean) (st->member.access == ACCESS_PRIVATE),
				TAG_EVENT);
	else if (st->declaration == DECL_PACKAGE)
		tag_make (nameToken, st, FALSE, TAG_PACKAGE);
	else if (tag_valid_type_specifier (st->declaration))
	{
		if (st->notVariable)
			;
		else if (statement_check_member (st))
		{

			if (st->scope == SCOPE_GLOBAL  ||  st->scope == SCOPE_STATIC)
				tag_make (nameToken, st, TRUE, TAG_MEMBER);
		}
		else
		{
			if (st->scope == SCOPE_EXTERN  ||  ! st->haveQualifyingName)
				tag_make (nameToken, st, FALSE, TAG_EXTERN_VAR);
			else if (st->inFunction)
				tag_make (nameToken, st, (boolean) (st->scope == SCOPE_STATIC),
						TAG_LOCAL);
			else
				tag_make (nameToken, st, (boolean) (st->scope == SCOPE_STATIC),
						TAG_VARIABLE);
		}
	}
}


static void nest (statementInfo *const st, const unsigned int nestLevel)
{
	switch (st->declaration)
	{
		case DECL_ENUM:
		case DECL_STRUCT:
		case DECL_UNION:
			tag_create (nestLevel, st);
			break;

		case DECL_FUNCTION:
			st->inFunction = TRUE;
			/* fall through */
		default:
			if (ckind_get_enabled (TAG_LOCAL, FALSE))
				tag_create (nestLevel, st);
			else
				skipToMatch ("{}");
			break;
	}
	token_advance (st);
	token_set (st, TOKEN_BRACE_CLOSE);
}





static void tag_check (statementInfo *const st)
{
	const token_info *const token = activeToken (st);
	const token_info *const prev  = token_prev (st, 1);
	const token_info *const prev2 = token_prev (st, 2);
  //  printf(" token type %d, name : %s \r\n",token->type ,token->name->buffer);
	switch (token->type)
	{
		case TOKEN_NAME:
			if (insideEnumBody (st))
				tag_qualify_enumerator (st, token);
			break;
         
		case TOKEN_BRACE_OPEN:
			if (isType (prev, TOKEN_ARGS))
			{
				if (st->haveQualifyingName)
				{
					st->declaration = DECL_FUNCTION;
					tag_qualify_function (st, prev2);
				}
			}
			else if (statement_is_contextual (st) )
			{
				if (isType (prev, TOKEN_NAME)){
					token_copy (st->blockName, prev);
            }
				else
				{
					/*  For an anonymous struct or union we use a unique ID
					 *  a number, so that the members can be found.
					 */
					char buf [20];  /* length of "_anon" + digits  + null */
					sprintf (buf, "__anon%d", ++AnonymousID);
					string_copy(st->blockName->name, buf);
					st->blockName->type = TOKEN_NAME;
					st->blockName->keyword = KEYWORD_NONE;
				}
				tag_qualify_block (st, prev);
			}
			break;

		case TOKEN_SEMICOLON:
		case TOKEN_COMMA:
			if (insideEnumBody (st))
				;
			else if (isType (prev, TOKEN_NAME))
			{
				if (token_is_contextual_keyword (prev2))
					tag_make (prev, st, TRUE, TAG_EXTERN_VAR);
				else
					tag_qualify_variable (st, prev);
			}
			else if (isType (prev, TOKEN_ARGS)  &&  isType (prev2, TOKEN_NAME))
			{
				if (st->isPointer)
					tag_qualify_variable (st, prev2);
				else
					tag_qualify_function_decl (st, prev2);
			}

			break;

		default: break;
	}
}

/*  Parses the current file and decides whether to write out and tags that
 *  are discovered.
 */
void tag_create (const unsigned int nestLevel,
						statementInfo *const parent)
{
	statementInfo *const st = statement_new (parent);
  


	while (TRUE)
	{
		token_info *token;

		token_next (st);
		token = activeToken (st);
		if (isType (token, TOKEN_BRACE_CLOSE))
		{
			if (nestLevel > 0)
				break;
			else
			{
				printf ("%s: unexpected closing brace at line %lu\n",
						get_string_value(g_file.name), g_file.line_number);
				longjmp (Exception, (int) ExceptionBraceFormattingError);
			}
		}
		else if (isType (token, TOKEN_DOUBLE_COLON))
		{
			token_add_context (st, token_prev (st, 1));
			token_advance (st);
		}
		else
		{
			tag_check (st);
			if (isType (token, TOKEN_BRACE_OPEN))
				nest (st, nestLevel + 1);
			statement_check_end (st);
		}
	}
  
 // printf("st->blockName %s  \r\n",st->blockName->name->buffer);

    statement_delete();

}

extern boolean tag_finding()
{
    exception_t exception;
	boolean retry;

	Signature = string_new();

	exception = (exception_t) setjmp (Exception);
	retry = FALSE;
	if (exception == ExceptionNone)
		tag_create (0, NULL);
	else
	{
		statement_delete_all();
		if (exception == ExceptionBraceFormattingError)
		{
			retry = TRUE;
		   printf ("%s: retrying file with fallback brace matching algorithm\n",
					get_string_value(g_file.name));
		}
	}
	string_delete(Signature);
	return retry;
}

extern  void build_keyword_hashtable ()
{
	const size_t count = sizeof (KeywordTable) / sizeof (KeywordTable [0]);
	size_t i;
	for (i = 0  ;  i < count  ;  ++i)
	{
		const keywordDesc* const p = &KeywordTable [i];
      keyword_add (p->name, (int) p->id);
	}
  
}




