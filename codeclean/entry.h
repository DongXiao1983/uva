#ifndef _ENTRY_H_
#define _ENTRY_H_

#include "xstring.h"
/*
*   DATA DECLARATIONS
*/

typedef struct entry_st {
   struct entry_st *next;
   struct entry_st *prev;
   struct entry_st *member;

   xString    *name;            
   xString    *type;
   xString    *value;           
   xString    *typeRef[2];      /* if this is a typedef define*/
   tag_type   ttagtype;
   keywordId  keyword;
   int        isPoint;
   int        isArray;
   int        array_count;       /* if this is an array struct , default is 1*/
   int        max_member_size;   /* Used in mix-struct, the struct's align with its max member size */
   int        size;              /* size, include * by array_count */
   int        offset;            /* memory start posion */
   boolean    isroot;
} entry_info;



typedef struct entry_list_st {
	unsigned int     max;
	unsigned int   count;
	entry_info    **list;
} entry_list;


/*
*   FUNCTION PROTOTYPES
*/

extern entry_info* entry_new(xString *name);

extern void entry_list_init(void);

extern void entry_list_add(entry_info* et);
   
extern void entry_list_add_parent(entry_info* et);

extern void entry_list_add_child(entry_info* et, unsigned int index);

extern void entry_list_remove_last(void);

extern void entry_list_delete(entry_info* st);

extern void entry_list_clear();

extern entry_info *entry_list_item(unsigned int index);

extern unsigned int entry_list_count(void);

extern void entry_list_print();  
 
void entry_info_print(entry_info* et, int level, int offset);

extern char *entry_list_query_item(char *name);

extern void entry_list_sizeof_entry();

extern void entry_map_to_rawdata(const xString* const struct_name);

extern void entry_info_pre_marco(xString **value);


#endif

