#include "common.h"
#include "xstring.h"
#include "chead.h"
#include "entry.h"
#include "memory.h"
#include <string.h>
#include "keyword.h"
#include <stdarg.h>

entry_list *g_entry_list = NULL;

extern char *g_bin_buff;

boolean  entry_list_is_emptry()
{
    
   if ( g_entry_list == NULL )
      return TRUE;

   if (g_entry_list->list == NULL )
      return TRUE;
   return FALSE;
}

extern entry_info* entry_new(xString *name)
{
   entry_info *entry = xMalloc(1, entry_info ); 
   memset(entry,0,sizeof(entry_info));
   entry->name        =  string_new_init(get_string_value(name));
   entry->type        =  string_new();
   entry->value       =  string_new();
   entry->typeRef[0]  =  string_new();
   entry->typeRef[1]  =  string_new();
   entry->array_count =  1;
   entry->next        =  NULL;
   entry->prev        =  NULL;
   return entry;
}

extern void entry_list_init(void)
{
   entry_list* et = xMalloc(1, entry_list);
   et->max = 0;
   et->count = 0;
   et->list = NULL;
   g_entry_list =  et;
}

extern void entry_list_add(entry_info* et)
{
   if (et->isroot )
      entry_list_add_parent(et);
   else
      entry_list_add_child(et, entry_list_count()? (entry_list_count()-1):0);
}

extern void entry_list_add_parent(entry_info* et)
{
    enum { incrementa = 10 };
    if ( g_entry_list == NULL ){
      g_entry_list->count = 0;
      g_entry_list->max   = incrementa ;
      g_entry_list->list  = xMalloc(g_entry_list->max,entry_info*);
    }
    else if ( g_entry_list->count == g_entry_list->max ) {
      g_entry_list->max += incrementa;
      g_entry_list->list = xRealloc(g_entry_list->list, g_entry_list->max,entry_info*);
    }

    g_entry_list->list[g_entry_list->count++] = et;
    et->max_member_size = 0;
    
}

extern void entry_list_add_child(entry_info* et, unsigned int index)
{
   entry_info *head = NULL;
   
   if ( index > g_entry_list->max )
      return;
   
   if ( g_entry_list->list == NULL ){
      entry_list_add_parent(et);
	  return;
   }
   
   head = g_entry_list->list[index];
   
   if ( head == NULL ) {
      entry_list_add_parent(et);
	  return;
   }
   
   if ( (et->size/et->array_count ) > head->max_member_size )
      head->max_member_size = et->size/et->array_count;
 
   while ( head != NULL && head->next != NULL)
      head = head->next;
   
   head->next = et;
   et->prev = head;
}


extern void entry_list_remove_last(void)
{
   g_entry_list->count--;
   g_entry_list->list[g_entry_list->count] = NULL;
}


extern void entry_list_delete(entry_info* et)
{
   int i = 0;
   if ( g_entry_list != NULL) {
      if ( g_entry_list->list != NULL ) {
         for ( i = 0 ; i < g_entry_list->count ; i++ ){
            if ( string_compare(g_entry_list->list[i]->name, et->name)){
               xfree(g_entry_list->list[i]);
               break;
            }
         }
  
         for ( ; i < g_entry_list->count ; i++ ){
            g_entry_list->list[i]= g_entry_list->list[i+1];
         }
         g_entry_list->list[i] = NULL;
         g_entry_list->count--;
      }
   }
}

extern entry_info *entry_list_find_root(const xString *const name, tag_type type)
{
   int i = 0;
   for ( i = 0 ; i < g_entry_list->count ; i++ ){
      if ( string_compare(g_entry_list->list[i]->name, name) && 
            g_entry_list->list[i]->ttagtype == type)
         return g_entry_list->list[i];
   }
   return NULL;
}

extern void entry_list_clear(){}

extern entry_info *entry_list_item(unsigned int index)
{
    if ( g_entry_list != NULL && g_entry_list->list != NULL && index < g_entry_list->max ) {
       return g_entry_list->list[index];
    }
    return NULL;
}

extern char *entry_list_query_item(char *name)
{
   int i = 0;
   entry_info *parent, *child;

   if ( entry_list_is_emptry() )  
      return NULL;

   for ( i = 0 ; i < g_entry_list->count ; i ++ ) {
      if ( g_entry_list->list[i] == NULL ) {
         printf(" A error in G_ENTRY_LIST %d \n", i);
         continue;
      }
      child = parent = g_entry_list->list[i];
      do {
         if(strcmp(parent->name->buffer,name)==0)
            return parent->type->buffer;

         child = child->next;
      }while(child);
   }
   return NULL;
}

extern entry_info * entry_list_find_typedef_define(entry_info * parent)
{
   xString *type = string_new_init(parent->typeRef[0]->buffer);
   xString *name = string_new_init(parent->typeRef[1]->buffer);
   unsigned int     index = 0;

   for ( index=0 ; index < g_entry_list->count ; index++ ){
      if (string_compare(name,g_entry_list->list[index]->name) && 
         string_compare(type,g_entry_list->list[index]->type))
         return g_entry_list->list[index];
   }
   return parent;
}

extern unsigned int entry_list_count(void)
{
   return g_entry_list->count;
}


extern unsigned int entry_list_query_child_size(xString *type)
{
   int        index = 0;
   unsigned int ret = 0;  
   if ( entry_list_is_emptry() )  
      ret =  0;
 
   for ( index=0 ; index < g_entry_list->count ; index++ ){
      if (string_compare(type,g_entry_list->list[index]->name) == 1) {
         ret =  g_entry_list->list[index]->offset;
         break;
      }
   }
   return ret;
}


static entry_info * entry_list_query_parent(const xString *const name)
{
   int        index = 0;
 
   if ( entry_list_is_emptry() )  
      return NULL;
 
   for ( index=0 ; index < g_entry_list->count ; index++ ){
      if (string_compare(name,g_entry_list->list[index]->name) == 1 ) {
         if ( g_entry_list->list[index]->ttagtype == TAG_TYPEDEF ) {
            return entry_list_query_parent(g_entry_list->list[index]->typeRef[1]);
         }
         else  {
            return g_entry_list->list[index];
         }
      }
   }
   
   return NULL;
}

extern void entry_list_print()
{
   int i = 0;
   entry_info *parent, *child;
   
   if ( entry_list_is_emptry() )  
      return;

   for ( i = 0 ; i < g_entry_list->count ; i ++ ) {
        if ( g_entry_list->list[i] == NULL ) {
            printf(" A error in G_ENTRY_LIST %d \n", i);
            continue;
         }
         parent = g_entry_list->list[i];
          //if ( parent->ttagtype == TAG_TYPEDEF ) continue;
          printf( "-------: offet : %d , type: %d  %s ( %s ) ( %s , %s )\n"  ,
            parent->offset,  parent->ttagtype, 
            parent->name->buffer, parent->type->buffer, 
            parent->typeRef[0]->buffer,parent->typeRef[1]->buffer);
          
         child = parent->next;
         while (child){
            printf( "   |------: offet : %d  %s ( %s: %d : %d) ( %s , %s)  \n"  ,
               child->offset,child->name->buffer, 
               child->type->buffer,child->size , child->array_count,
               child->typeRef[0]->buffer,child->typeRef[1]->buffer);
            
            child = child->next;
         }
    }
         
   
}

static void entry_list_sizeof_typedef(entry_info *et)
{
   et->offset = entry_list_find_typedef_define(et)->offset;
}

static void entry_list_sizeof_enum(entry_info *et)
{
   et->offset = 4;
}

static void entry_list_sizeof_union(entry_info *et)
{
   entry_info   *child      = et->next;;
   unsigned int  big_size   = 0;
   unsigned int  align_size = 0;
   
   while(child != NULL ){
      
      if ( child->size == 0){
         child->size = entry_list_query_child_size(child->type);   
      }  
      
      SET_MAX_VALUE (align_size, child->size / child->array_count);
     
      SET_MAX_VALUE ( big_size , child->size);
 
      child = child->next;
   }

   et->offset = ALIGN(big_size,align_size);

}

static void entry_list_sizeof_struct(entry_info *et)
{
   entry_info   *child      = NULL;
   unsigned int  offset     = 0;
   unsigned int  big_size   = 0;
   unsigned int  align_size = 0;
   
   for ( child = et->next ; child != NULL ; child=child->next ) {

      if ( child->keyword != KEYWORD_NONE) {
      /* struct align law 1: Every member shoud start its size *n */

         offset = ALIGN(offset, child->size/child->array_count);  

         align_size = child->size/child->array_count;
      }
      else {
         /* struct align law 2: Every STRUCT member shoud start its MAX member's size *n */  
         entry_info *father =  entry_list_query_parent(child->type); 
         child->size = father ? father->offset : 0;

         if ( father == NULL ) continue;
            
          
         if (father->ttagtype == TAG_STRUCT ) {
            offset = ALIGN(offset, father->max_member_size);
            align_size = father->max_member_size; 
         }
         else {
            
            if (child->size == 0 ){ // for system type , such lisk  fpos_t
               child->size=sizeof(child->type->buffer);   
            }
            
            offset = ALIGN(offset, child->size/child->array_count); 
       
            align_size = child->size;
         }
            

      } 

      child->offset = offset;
      offset += child->size;
      
      SET_MAX_VALUE(big_size, align_size);


   }
 
   et->offset =ALIGN(offset, big_size );

}



extern void entry_list_sizeof_entry()
{
   int i = 0;

   entry_info *parent;

   if ( entry_list_is_emptry() )  
      return;

   for ( i = 0 ; i < g_entry_list->count ; i ++ ) {
      if ( g_entry_list->list[i] == NULL ) {
         printf(" A error in G_ENTRY_LIST %d \n", i);
         continue;
      }
      parent = g_entry_list->list[i];
      
      switch (parent->ttagtype) {
         case TAG_TYPEDEF: 
            entry_list_sizeof_typedef(parent); break;
            
         case TAG_ENUM:   
            entry_list_sizeof_enum(parent);    break;
            
         case TAG_UNION:
            entry_list_sizeof_union(parent);   break;
            
         case TAG_STRUCT:
            entry_list_sizeof_struct(parent); break;
            
         default :
            break;
      }

   }
}





extern void print_with_indent( int indent, const char* format, ...)
{
   va_list ap;
   
   va_start(ap, format);
   while(indent--){
      fprintf( stderr, "   ");
   }
   vfprintf( stderr, format, ap);

   va_end(ap);

}


#define define_type(type,mem,offset)    type *value = (type *)(mem+offset);

#define entry_print_value_char     printf("%d, 0x%x ",         *value,*value); ( *value ) ? ( printf( ", '%c'  \n",*value) ): ( printf( "  \n"));
#define entry_print_value_short    printf("%d, 0x%x\n",        *value,*value)
#define entry_print_value_int      printf("%d, 0x%x\n",        *value,*value)
#define entry_print_value_long     printf("%ld (long) \n",     *value)
#define entry_print_value_float    printf("%f  (float)\n",     *value)
#define entry_print_value_double   printf("%lf (double) \n",   *value)



#define  entry_print_array_value(type,  member, wide, level, base)       \
   \
do{                                                                   \
   int i = 0;                                                        \
   print_with_indent(level+1,"%s = [\n",member->name->buffer);      \
                                                                     \
   for ( i = 0 ; i< member->size / wide;i++){                        \
      define_type(type, g_bin_buff, member->offset+base+i*wide);          \
      print_with_indent((level+2),"[%d] = ",i); \
      entry_print_value_##type;\
   }                                                                        \
   print_with_indent((level+1),"]\n");                                       \
}while(0);




#define entry_print_single_value(type,  member,  level,  base) \
   \
do { \
   define_type(type, g_bin_buff , member->offset+base ); \
   print_with_indent(level+1,"%s = ",member->name->buffer); \
   entry_print_value_##type ;\
}while(0);



void entry_print_embed_member_value(entry_info * def,int level,tag_type type,int base)
{

   if ( type == TAG_ENUM ){
      
      entry_info_print(def, level, base);
      
   }
   else {
      printf("%s %s {\n",   get_string_value(def->type),get_string_value(def->name));
      
      entry_info_print(def, level, base);
      
      print_with_indent(level, "}\n"); 
   }

}

void entry_print_embed_member_single(entry_info* member, entry_info* def, 
                                             int level, tag_type type ,int base)
{
   print_with_indent(level+1,"%s = ",get_string_value(member->name));

   entry_print_embed_member_value(def,level+1,type, member->offset+base);

}



void entry_print_embed_member_array(entry_info* member, entry_info* def, 
                                             int level, tag_type type ,int base)
{
   int index = 0;
   
   print_with_indent(level+1,"%s = [\n",get_string_value(member->name));

   for ( index = 0 ; index < member->array_count ; index++) {
      
      print_with_indent((level+2), "[%d] = ", index);
      
      entry_print_embed_member_value(def,level+2,type,member->offset + base + member->size*index);

    }
   
    print_with_indent(level+1, "]\n"); 

}



void entry_print_embed_member(entry_info* member, entry_info* def, 
                                             int level, tag_type type ,int base)
{

   if ( member->isArray ) 
      entry_print_embed_member_array( member,def, level, type, base);
   else
      entry_print_embed_member_single( member,def, level, type, base);
 
}

extern void entry_info_pre_marco(xString **value)
{
   int i = 0;
   entry_info *marco;
   for ( i = 0 ; i < g_entry_list->count ; i ++ ) {
      
      if ( g_entry_list->list[i] == NULL 
         || g_entry_list->list[i]->ttagtype != TAG_DEFINE) {
         continue;
      }
      
      marco = g_entry_list->list[i];
      
      if ( string_compare(marco->name,*value) ){
         string_copy(*value,marco->value->buffer);         
      }
   }
}


static void entry_print_emun_value(const entry_info* et, const int offset)
{
   entry_info *member = NULL;

   int* value = (int*)(g_bin_buff + offset);
   
   for ( member = et->next; member; member= member->next) {
      
      entry_info_pre_marco(&(member->name));

      if (atoi(string_value(member->name)) == *value) {
         printf("%s (%d, 0x%x)\n",string_value(member->prev->name),*value,*value);
         break;
      }
   }
}


/* 
   level  : the indent of tabs
   offset : the strunct's base memory address
 */
void entry_info_print(entry_info* et, int level, int offset)
{
   entry_info *member = NULL;
   int wide;
   int base = offset;
   
   if ( et->ttagtype == TAG_ENUM ){
      entry_print_emun_value(et,offset);
      return;
   }
   
   for ( member = et->next; member !=NULL ; member= member->next) {
     
      if ( is_keyword(member->type->buffer,&wide) ) {

         if ( member->isArray ){   
            
            switch  (member->keyword ) {

               case KEYWORD_CHAR:    entry_print_array_value(char,member, wide, level, base);     break;
               case KEYWORD_SHORT:   entry_print_array_value(short, member, wide, level, base);   break;
               case KEYWORD_INT:     entry_print_array_value(int, member, wide, level, base);     break; 
               case KEYWORD_LONG:    entry_print_array_value(long, member, wide, level, base);    break; 
               case KEYWORD_FLOAT:   entry_print_array_value(float, member, wide, level, base);   break; 
               case KEYWORD_DOUBLE:  entry_print_array_value(double, member, wide, level, base);  break; 

               default: break;
            }
         }
         else {
            switch (member->keyword) {
         
               case KEYWORD_CHAR:    entry_print_single_value(char, member, level, base);    break; 
               case KEYWORD_INT:     entry_print_single_value(int, member, level, base);     break;  
               case KEYWORD_SHORT:   entry_print_single_value(short, member, level, base);   break; 
               case KEYWORD_LONG:    entry_print_single_value(long, member, level, base);    break; 
               case KEYWORD_FLOAT:   entry_print_single_value(float, member, level, base);   break; 
               case KEYWORD_DOUBLE:  entry_print_single_value(double, member, level, base);  break; 

               default: break;
            }

         }   
      }
      else {
         
         entry_info *define      =  entry_list_query_parent(member->type);
         keywordId   type        =  keyword_lookup(string_value(define->type));

         switch (type) {
            case KEYWORD_STRUCT:
               entry_print_embed_member( member,  define, level, TAG_STRUCT ,base);
               break;
            case  KEYWORD_UNION:
               entry_print_embed_member( member,  define, level, TAG_UNION ,base);
               break;
            case KEYWORD_ENUM:
               entry_print_embed_member( member,  define, level, TAG_ENUM ,base);
               break;
            default: break;
         }

      }
   }
}

extern void entry_map_to_rawdata(const xString* const struct_name )
{
   entry_info *parent;
   
   if (entry_list_is_emptry() ) {
      fputs ("Pre-process head faild.\n",stderr);
   }      

   parent= entry_list_query_parent(struct_name);
   
   if ( parent == NULL ) {
      fprintf( stderr, "WARNING: No such struct '%s' was found.\n" , struct_name->buffer);   
      return;
   }
   
   print_with_indent(0, "%s %s {\n",parent->type->buffer,parent->name->buffer);
   
   entry_info_print(parent,0, 0);
   
   print_with_indent(0, "}\n");
}

