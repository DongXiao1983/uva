#include "CppUTest/UtestMacros.h" 
#include "CppUTest/SimpleString.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

extern "C" {

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "xstring.h"
#include "chead.h"
#include "entry.h"

}

extern  entry_list* g_entry_list ;


TEST_GROUP(Test_entry)
{
   void setup()
   {
      entry_list_init();
   }

   void teardown()
   {
   
      unsigned int i = 0 ;
      
      if ( g_entry_list == NULL )
         return;
      if ( g_entry_list->list == NULL)
         return;
      
      for ( i = 0 ; i < g_entry_list->count ; i++ ){
         entry_info *p = g_entry_list->list[i];
         entry_info *c = p->next;
         while(c!=NULL){
            string_delete(c->name);
            string_delete(c->type);
            string_delete(c->typeRef[0]);
            string_delete(c->typeRef[1]);
            p->next = c->next;
            free(c);
            c = p->next;
         }
      }
     
      
   }
};



TEST(Test_entry, e_list_init)
{
   entry_list_init();
   CHECK_TRUE(g_entry_list);
}


TEST(Test_entry, e_new)
{
   xString *name = string_new_init("new entry");
   entry_info *ei = entry_new(name);
   CHECK_TRUE(string_compare(ei->name,name));  

}


TEST(Test_entry, e_add_p)
{
   xString *name = string_new_init("new entry");
   entry_info *ei = entry_new(name);
   
   entry_list_add_parent(ei);
   CHECK_TRUE(string_compare(g_entry_list->list[0]->name,name));  

   string_delete(name);
}


TEST(Test_entry, e_add_c)
{
   xString *name_parent = string_new_init("new entry parent");
   xString *name_child = string_new_init("new entry child ");
   entry_info *ep = entry_new(name_parent);
   entry_info *ec = entry_new(name_child);
   int index = 0;
   
   
   entry_list_add_parent(ep);
   entry_list_add_child(ec,index);
   
   CHECK_TRUE(string_compare(g_entry_list->list[index]->next->name,name_child));  

   string_delete(name_parent);
   string_delete(name_child);
}


TEST(Test_entry,e_delete)
{
   xString *name_1 = string_new_init("new entry 1");
   xString *name_2 = string_new_init("new entry 2");


   entry_info *entry_1 = entry_new(name_1);
   entry_info *entry_2 = entry_new(name_2);

   entry_list_add_parent(entry_1);
   entry_list_add_parent(entry_2);

   
   CHECK_EQUAL(g_entry_list->count,2);
   
   entry_list_delete(entry_2);

   CHECK_EQUAL(g_entry_list->count,1);


}


