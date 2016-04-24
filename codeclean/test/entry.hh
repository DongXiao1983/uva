

/*  Information about the current tag candidate.
 */
struct test{
	int a ;
	int b;
	char c[3];
};
   
typedef enum abool { CC, DD } bool;

typedef struct sTagEntryInfo {
	bool      lineNumberEntry;  
	unsigned long lineNumber;     
	//fpos_t      filePosition;   
	const char* language;       
	bool     isFileScope;      
	bool     isFileEntry;      
	bool     truncateLine;     
	const char *sourceFileName; 
	const char *name;             
	const char *kindName;       
	char        kind;             
   struct test t;
	struct {
		const char* access;
		const char* fileScope;
		const char* implementation;
		const char* inheritance;
		const char* scope [2];   
		const char* signature;

		const char* typeRef [2];  

	} extensionFields; 
} tagEntryInfo;






