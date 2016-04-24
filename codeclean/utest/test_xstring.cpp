#include "CppUTest/UtestMacros.h" 
#include "CppUTest/SimpleString.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

extern "C" {

#include <stdio.h>
#include <string.h>
#include "xstring.h"

}

static const char* const g_string              = "A quick brown fox jump over the lazy dog.";

static const char* const g_string_uppcase      = "A QUICK BROWN FOX JUMP OVER THE LAZY DOG.";

static const char* const g_string_lowcase      = "a quick brown fox jump over the lazy dog.";

static const char* const g_string_fron_half    = "A quick brown fox ";

static const char* const g_string_back_half    = "jump over the lazy dog.";


TEST_GROUP(Test_xString)
{

};



TEST(Test_xString, s_new_init)
{

   xString* a = string_new_init(g_string);
   STRCMP_EQUAL(a->buffer,g_string);
   string_delete(a);

};


TEST(Test_xString, s_sub)
{
   const char* const value  = g_string;
   const char* const expect = "brown fox jump over the lazy dog.";
   const int  start = 8;
   const int  end   = strlen(value);
   xString* string = string_new_init(value);
   string = string_substr(string, start, end);
   STRCMP_EQUAL(string->buffer,expect);   
   string_delete(string);

};


TEST(Test_xString, s_cmp)
{
   xString* src  = string_new_init(g_string);
   xString* dest = string_new_init(g_string);
   int ret = string_compare(src, dest);
   CHECK(ret==true);   

   string_delete(dest);
   dest =string_new_init(g_string_uppcase); 
   ret = string_compare(src, dest);
   CHECK(ret==false);  

   string_delete(dest);
   string_delete(src);
}


TEST(Test_xString, s_i_cmp)
{
   xString* src  = string_new_init(g_string);
   xString* dest = string_new_init(g_string);
   int ret       = string_i_compare(src, dest);
   CHECK(ret==true);   

   string_delete(dest);
   
   dest =string_new_init(g_string_uppcase); 
   ret  = string_i_compare(src, dest);
   CHECK(ret==true);

   
   string_delete(dest);
   string_delete(src);

}



TEST(Test_xString, s_copy_to_lower)
{
   xString *dest = string_new();
   xString *src  = string_new_init(g_string_uppcase);
   
   string_copy_to_lower (dest, src);

   STRCMP_EQUAL(dest->buffer, g_string_lowcase);
 
   string_delete(dest);
   string_delete(src); 
}



TEST(Test_xString, s_resise)
{
   
   xString* string = string_new_init(g_string);
   unsigned int size1 = string->size;
   unsigned int size2 = 0;

   
   string_auto_resize(string);
   size2 = string->size;
   
   CHECK_EQUAL(size1 *2 ,size2);  

   string_delete(string);

}

TEST(Test_xString, s_cat)
{
   xString* string = string_new_init(g_string_fron_half);
   unsigned int length_fh = strlen(g_string_fron_half);
   unsigned int length_bh = strlen(g_string_back_half);
   
   string_cat(string, g_string_back_half);

   
   STRCMP_EQUAL(string->buffer, g_string );
   CHECK_EQUAL(string->length, length_fh+ length_bh);

   string_delete(string);

}







