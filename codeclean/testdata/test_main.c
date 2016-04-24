#include <stdio.h>
#include <stdlib.h>
#include "Test_MemAlign.h"
#include "Test_Marco.h"
#include "Test_MutilMarco.h"
#include "Test_StructArray.h"
void init_test_mem_align()
{
   FILE *fp;
	int i = 0;
	fp = fopen("Test_MemAlign.bin","wb");

	struct MemAlign mem = {0};

        mem.a = 1;
	for( i = 0; i < 10 ;i++)
		mem.b[i] = 'G'+i;
	mem.c = 255;
	mem.d = 999.99;
	mem.as.a = 11;
	mem.as.b = 'a';
	mem.as.c = 0xFF;
	mem.e   = 100;
	mem.bs.a = 21;
	mem.bs.b = 'b';
	for ( i = 0 ; i < 10 ; i++)
		mem.bs.c[i] = 100+i;

	fwrite(&mem, sizeof(mem),1,fp);
	printf(" mem size %d \n", sizeof(mem));
}

void init_test_marco()
{
   FILE *fp;
	int i = 0;
	fp = fopen("Test_Marco.bin","wb");

	struct Marco mem = {0};

   
	for( i = 0; i < SIZE ;i++)
		mem.size[i] = i;

   for( i = 0; i < LENGTH ;i++)
		mem.length[i] = 'a'+i;

   for( i = 0; i < WIDE ;i++)
		mem.wide[i] = 100+i;
   
   for( i = 0; i < HIGH ;i++)
		mem.high[i] = 'l'+i;

   for( i = 0; i < DEEP ;i++)
		mem.deep[i] = 'A'+i; 
   
   for( i = 0; i < EXP ;i++)
		mem.exp[i] = 200+i; 
      
	fwrite(&mem, sizeof(mem),1,fp);
	printf(" mem size %d \n", EXP);
}

void init_test_mutil_marco()
{
   FILE *fp;
        int i = 0;
        fp = fopen("Test_MutilMarco.bin","wb");

        struct MutilMarco mem = {0};


        for( i = 0; i < SIZE ;i++)
                mem.size[i] = i;

   for( i = 0; i < LENGTH ;i++)
                mem.length[i] = 'a'+i;

   for( i = 0; i < WIDE ;i++)
                mem.wide[i] = 100+i;

   for( i = 0; i < HIGH ;i++)
                mem.high[i] = 'l'+i;

   for( i = 0; i < DEEP ;i++)
                mem.deep[i] = 'A'+i;

   for( i = 0; i < EXP ;i++)
                mem.exp[i] = 200+i;

        fwrite(&mem, sizeof(mem),1,fp);
        printf(" mem size %d \n", EXP);
}



void init_test_struct_array()
{
  FILE *fp;
  fp = fopen("Test_StructArray.bin","wb");
  struct StructArray sa = {0};
  int i = 0;
  sa.size = 10;
  for ( i - 0 ; i < 10 ; i++ ){
	 sa.member[i].name = 'a' + i;
    sa.member[i].age  = 10 +i;
  }
  fwrite(&sa, sizeof(sa),1,fp);
  printf(" mem size %d \n", sizeof(sa));
}


int main(int argc , char **argv)
{
/*
   init_test_mem_align();

   init_test_marco();
  
   init_test_mutil_marco();
   
   init_test_struct_array();
*/
   return 0;
}
