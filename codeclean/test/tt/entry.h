#ifndef _ENTRY_H_
#define _ENTRY_H_

/*  Information about the current tag candidate.
 */
struct test{
	int a ;
	int b;
	char c[4];
};
typedef enum boolean { FALSE, TRUE } bb;
typedef struct sTagEntryInfo {
	boolean     lineNumberEntry;  /* pattern or line number entry */
	unsigned long lineNumber;     /* line number of tag */
	fpos_t      filePosition;     /* file position of line containing tag */
	const char* language;         /* language of source file */
	boolean     isFileScope;      /* is tag visibile only within source file? */
	boolean     isFileEntry;      /* is this just an entry for a file name? */
	boolean     truncateLine;     /* truncate tag line at end of tag name? */
	const char *sourceFileName;   /* name of source file */
	const char *name;             /* name of the tag */
	const char *kindName;         /* kind of tag */
	char        kind;             /* single character representation of kind */
        struct test t;
	struct {
		const char* access;
		const char* fileScope;
		const char* implementation;
		const char* inheritance;
		const char* scope [2];    /* value and key */
		const char* signature;

		/* type (union/struct/etc.) and name for a variable or typedef. */
		const char* typeRef [2];  /* e.g., "struct" and struct name */

	} extensionFields;  /* list of extension fields*/
} tagEntryInfo;





#endif

