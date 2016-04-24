#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTestExt/MockSupportPlugin.h"
#include "CppUTest/TestRegistry.h"

extern "C" {
#include <stdio.h>
}


int main(int argc , char** argv)
{
   MockSupportPlugin mockPlugin;
   TestRegistry::getCurrentRegistry()->installPlugin(&mockPlugin);
   int res = CommandLineTestRunner::RunAllTests(argc, argv);
   TestRegistry::getCurrentRegistry()->resetPlugins();
   return res;
}
