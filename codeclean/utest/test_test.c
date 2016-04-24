#include "CppUTest/UtestMacros.h" 
#include "CppUTest/SimpleString.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

extern "C" {

#include <stdio.h>
#include "xstring.h"

}

TEST_GROUP(FirstTestGroup)
{
};



TEST(FirstTestGroup, FirstTest)
{
   xString* a = string_new_init("string_new");
   STRCMP_EQUAL(a->buffer,"string_new");

}
