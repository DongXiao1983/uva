#ifndef _CHEAD_H_
#define _CHEAD_H_

enum { NumTokens = 3 };

typedef enum eException {
	ExceptionNone, 
   ExceptionEOF, 
   ExceptionFormattingError,
	ExceptionBraceFormattingError
} exception_t;


/*
*   MACROS
*/

#define activeToken(st)     ((st)->token [(int) (st)->tokenIndex])
#define parentDecl(st)      ((st)->parent == NULL ? \
                            DECL_NONE : (st)->parent->declaration)
#define isType(token,t)     (boolean) ((token)->type == (t))
#define insideEnumBody(st)  ((st)->parent == NULL ? FALSE : \
                            (boolean) ((st)->parent->declaration == DECL_ENUM))
#define isExternCDecl(st,c) (boolean) ((c) == STRING_SYMBOL  && \
                    ! (st)->haveQualifyingName  && (st)->scope == SCOPE_EXTERN)

#define isOneOf(c,s)        (boolean) (strchr ((s), (c)) != NULL)

#define isHighChar(c)       ((c) != EOF && (unsigned char)(c) >= 0xc0)

/* Used to index into the CKinds table. */
typedef enum {
	CK_UNDEFINED = -1,
	CK_CLASS, CK_DEFINE, CK_ENUMERATOR, CK_FUNCTION,
	CK_ENUMERATION, CK_LOCAL, CK_MEMBER, CK_NAMESPACE, CK_PROTOTYPE,
	CK_STRUCT, CK_TYPEDEF, CK_UNION, CK_VARIABLE,
	CK_EXTERN_VARIABLE
} cKind;

/*  Describes the type of tag being generated.
 */
typedef enum tag_type_st {
	TAG_UNDEFINED,
	TAG_CLASS,       /* class name */
	TAG_ENUM,        /* enumeration name */
	TAG_ENUMERATOR,  /* enumerator (enumeration value) */
	TAG_EVENT,       /* event */
	TAG_FUNCTION,    /* function definition */
	TAG_INTERFACE,   /* interface declaration */
	TAG_LOCAL,       /* local variable definition */
	TAG_MEMBER,      /* structure, class or interface member */
	TAG_METHOD,      /* method declaration */
	TAG_NAMESPACE,   /* namespace name */
	TAG_PACKAGE,     /* package name */
	TAG_PROGRAM,     /* program name */
	TAG_PROPERTY,    /* property name */
	TAG_PROTOTYPE,   /* function prototype or declaration */
	TAG_STRUCT,      /* structure name */
	TAG_DEFINE,      /* DEFINE name */
	TAG_TASK,        /* task name */
	TAG_TYPEDEF,     /* typedef name */
	TAG_UNION,       /* union name */
	TAG_VARIABLE,    /* variable definition */
	TAG_EXTERN_VAR,  /* external variable declaration */
	TAG_COUNT        /* must be last */
} tag_type;


/*  Used to specify type of keyword.
 */
typedef enum eKeywordId {
	KEYWORD_NONE = -1,
	KEYWORD_ATTRIBUTE, KEYWORD_ABSTRACT,
	KEYWORD_BOOLEAN, KEYWORD_BYTE, KEYWORD_BAD_STATE, KEYWORD_BAD_TRANS,
	KEYWORD_BIND, KEYWORD_BIND_VAR, KEYWORD_BIT,
	KEYWORD_CASE, KEYWORD_CATCH, KEYWORD_CHAR, KEYWORD_CLASS, KEYWORD_CONST,
	KEYWORD_CONSTRAINT, KEYWORD_COVERAGE_BLOCK, KEYWORD_COVERAGE_DEF,
	KEYWORD_DEFAULT, KEYWORD_DELEGATE, KEYWORD_DELETE, KEYWORD_DO,
	KEYWORD_DOUBLE,
	KEYWORD_ELSE, KEYWORD_ENUM, KEYWORD_EXPLICIT, KEYWORD_EXTERN,
	KEYWORD_EXTENDS, KEYWORD_EVENT,
	KEYWORD_FINAL, KEYWORD_FLOAT, KEYWORD_FOR, KEYWORD_FOREACH,
	KEYWORD_FRIEND, KEYWORD_FUNCTION,
	KEYWORD_GOTO,
	KEYWORD_IF, KEYWORD_IMPLEMENTS, KEYWORD_IMPORT, KEYWORD_INLINE, KEYWORD_INT,
	KEYWORD_INOUT, KEYWORD_INPUT, KEYWORD_INTEGER, KEYWORD_INTERFACE,
	KEYWORD_INTERNAL,
	KEYWORD_LOCAL, KEYWORD_LONG,
	KEYWORD_M_BAD_STATE, KEYWORD_M_BAD_TRANS, KEYWORD_M_STATE, KEYWORD_M_TRANS,
	KEYWORD_MUTABLE,
	KEYWORD_NAMESPACE, KEYWORD_NEW, KEYWORD_NEWCOV, KEYWORD_NATIVE,
	KEYWORD_OPERATOR, KEYWORD_OUTPUT, KEYWORD_OVERLOAD, KEYWORD_OVERRIDE,
	KEYWORD_PACKED, KEYWORD_PORT, KEYWORD_PACKAGE, KEYWORD_PRIVATE,
	KEYWORD_PROGRAM, KEYWORD_PROTECTED, KEYWORD_PUBLIC,
	KEYWORD_REGISTER, KEYWORD_RETURN,
	KEYWORD_SHADOW, KEYWORD_STATE,
	KEYWORD_SHORT, KEYWORD_SIGNED, KEYWORD_STATIC, KEYWORD_STRING,
	KEYWORD_STRUCT, KEYWORD_SWITCH, KEYWORD_SYNCHRONIZED,
	KEYWORD_TASK, KEYWORD_TEMPLATE, KEYWORD_THIS, KEYWORD_THROW,
	KEYWORD_THROWS, KEYWORD_TRANSIENT, KEYWORD_TRANS, KEYWORD_TRANSITION,
	KEYWORD_TRY, KEYWORD_TYPEDEF, KEYWORD_TYPENAME,
	KEYWORD_UINT, KEYWORD_ULONG, KEYWORD_UNION, KEYWORD_UNSIGNED, KEYWORD_USHORT,
	KEYWORD_USING,
	KEYWORD_VIRTUAL, KEYWORD_VOID, KEYWORD_VOLATILE,
	KEYWORD_WCHAR_T, KEYWORD_WHILE
} keywordId;

/*  Used to determine whether keyword is valid for the current language and
 *  what its ID is.
 */
typedef struct sKeywordDesc {
	const char *name;
	keywordId id;
} keywordDesc;

static const keywordDesc KeywordTable [] = {
	/* keyword          keyword ID                */
	{ "__attribute__",  KEYWORD_ATTRIBUTE,       },
	{ "case",           KEYWORD_CASE,            },
	{ "char",           KEYWORD_CHAR,            },
	{ "const",          KEYWORD_CONST,           },
	{ "do",             KEYWORD_DO,              },
	{ "default",        KEYWORD_DEFAULT,         },
	{ "double",         KEYWORD_DOUBLE,          },
	{ "else",           KEYWORD_ELSE,            },
	{ "enum",           KEYWORD_ENUM,            },
	{ "extern",         KEYWORD_EXTERN,          },
	{ "float",          KEYWORD_FLOAT,           },
	{ "for",            KEYWORD_FOR,             },
	{ "goto",           KEYWORD_GOTO,            },
	{ "if",             KEYWORD_IF,              },
	{ "int",            KEYWORD_INT,             },
	{ "long",           KEYWORD_LONG,            },
	{ "register",       KEYWORD_REGISTER,        },
	{ "return",         KEYWORD_RETURN,          },
	{ "short",          KEYWORD_SHORT,           },
	{ "signed",         KEYWORD_SIGNED,          },
	{ "static",         KEYWORD_STATIC,          },
	{ "struct",         KEYWORD_STRUCT,          },
	{ "switch",         KEYWORD_SWITCH,          },
	{ "typedef",        KEYWORD_TYPEDEF,         },
	{ "union",          KEYWORD_UNION,           },
	{ "unsigned",       KEYWORD_UNSIGNED,        },
	{ "void",           KEYWORD_VOID,            },
	{ "volatile",       KEYWORD_VOLATILE,        },
	{ "wchar_t",        KEYWORD_WCHAR_T,         },
	{ "while",          KEYWORD_WHILE,           }
};

typedef struct type_size_t {
    const char *type;
    int  size;
}type_size;

static const type_size Keyword_type_size[] = {
    { "char",       sizeof(char) },
    { "int",        sizeof(int) },
    { "long",       sizeof(long) },   
    { "short",      sizeof(short)},
    { "*",          sizeof(void *) },
    { "float",      sizeof(float) },
    { "double",     sizeof(double) },
    { "long long",     sizeof(long long) },
    { "short int",    sizeof(short int)  },
    { "long double",  sizeof( long double)},
    { "enum",       sizeof( KEYWORD_ENUM)},
    { NULL, 0 }
};


/*  This describes the scoping of the current statement.
 */
typedef enum eTagScope {
	SCOPE_GLOBAL,        /* no storage class specified */
	SCOPE_STATIC,        /* static storage class */
	SCOPE_EXTERN,        /* external storage class */
	SCOPE_FRIEND,        /* declares access only */
	SCOPE_TYPEDEF,       /* scoping depends upon context */
	SCOPE_COUNT
} tagScope;

typedef enum eDeclaration {
	DECL_NONE,
	DECL_BASE,           /* base type (default) */
	DECL_CLASS,
	DECL_ENUM,
	DECL_EVENT,
	DECL_FUNCTION,
	DECL_IGNORE,         /* non-taggable "declaration" */
	DECL_PACKAGE,
	DECL_STRUCT,
	DECL_UNION,
	DECL_COUNT
} decl_type;

typedef enum eVisibilityType {
	ACCESS_UNDEFINED,
	ACCESS_LOCAL,
	ACCESS_PRIVATE,
	ACCESS_PROTECTED,
	ACCESS_PUBLIC,
	ACCESS_COUNT
} access_type;


/*  Used for reporting the type of object parsed by nextToken ().
 */
typedef enum eTokenType {
	TOKEN_NONE,          /* none */
	TOKEN_ARGS,          /* a parenthetical pair and its contents */
	TOKEN_BRACE_CLOSE,
	TOKEN_BRACE_OPEN,
	TOKEN_COLON,         /* the colon character */
	TOKEN_COMMA,         /* the comma character */
	TOKEN_DOUBLE_COLON,  /* double colon indicates nested-name-specifier */
	TOKEN_KEYWORD,
	TOKEN_NAME,          /* an unknown name */
	TOKEN_PAREN_NAME,    /* a single name in parentheses */
	TOKEN_SEMICOLON,     /* the semicolon character */
	TOKEN_SPEC,          /* a storage class specifier, qualifier, type, etc. */
	TOKEN_COUNT
} token_type;


typedef struct sTokenInfo {
	token_type    type;
	keywordId     keyword;
	xString*      name;          /* the name of the token */
	unsigned long lineNumber;    /* line number of tag */
	fpos_t        filePosition;  /* file position of line containing name */
} token_info;

/*  Information about the parent class of a member (if any).
 */
typedef struct sMemberInfo {
	access_type access;           /* access of current statement */
	access_type accessDefault;    /* access default for current statement */
} memberInfo;


/*  Describes the statement currently undergoing analysis.
 */
typedef struct sStatementInfo {
	tagScope	scope;
	decl_type	declaration;    /* specifier associated with TOKEN_SPEC */
	boolean		gotName;        /* was a name parsed yet? */
	boolean		haveQualifyingName;  /* do we have a name we are considering? */
	boolean		gotParenName;   /* was a name inside parentheses parsed yet? */
	boolean		gotArgs;        /* was a list of parameters parsed yet? */
	boolean		isPointer;      /* is 'name' a pointer? */
   boolean		isArray;        /* is 'name' a array? */
	boolean     inFunction;     /* are we inside of a function? */
	boolean		assignment;     /* have we handled an '='? */
	boolean		notVariable;    /* has a variable declaration been disqualified ? */
	unsigned int tokenIndex;    /* currently active token */
	token_info*	token [(int) NumTokens];
	token_info*	context;        /* accumulated scope of current statement */
	token_info*	blockName;      /* name of current block */
	memberInfo	member;         /* information regarding parent class/struct */
	xString*	parentClasses;  /* parent classes */
   xString*    arraysize;
   xString*    value;
	struct sStatementInfo *parent;  /* statement we are nested within */
} statementInfo;

/* statment function */




void tag_create (const unsigned int nestLevel,statementInfo *const parent);

extern boolean tag_finding ();

extern void build_keyword_hashtable ();

extern boolean is_keyword(char *name, int *size);


#endif

