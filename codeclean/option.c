#include "common.h"
#include "memory.h"
#include "xstring.h"
#include "option.h"
#include <unistd.h>

#define PROGRAM_NAME      "Log Raw Data Processer"
#define PROGRAM_VERSION   "1.0.0"
#define PROGRAM_COPYRIGHT "Copyright (C) 2013-2013"
#define AUTHOR_NAME       "Dong Xiao"

char *g_bin_buff;

const unsigned int opt_incraese = 10;

struct option_st g_option;

struct option_description_st option_description [] = {
   {"  Options:"},

   {"   -b  <bin file name>"},
   {"       The Raw data file'name."},
   {"   -i  <directory>"},
   {"       head file's directory"},
   {"   -f  <head file name>"},
   {"       head file's name"},
   {"   -s  <struct name>"},
   {"   -r  Recurse into directories."},
   {"   -h  Print this option summary."},
   {NULL}
};


void options_print_identification (void)
{
   putchar('\n');
	printf ("%s %s, %s %s\n",
	        PROGRAM_NAME, PROGRAM_VERSION,
	        PROGRAM_COPYRIGHT, AUTHOR_NAME);
	printf ("  Compiled: %s, %s\n", __DATE__, __TIME__);

}

void options_print_description(void)
{
   int i;
   for (i = 0 ; option_description [i].description != NULL ; ++i)
   {
      printf("%s \n",option_description[i].description);
   }
}


void options_print_help()
{
   options_print_identification();
   putchar('\n');
   options_print_description();
   putchar('\n');
   exit (0);
}


void options_init()
{
   g_option.recurse     = 0;
   g_option.dir_num     = 0;
   g_option.head_num    = 0; 
   g_option.bin_file    = string_new();
   g_option.struct_name = string_new();
   g_option.directory   = xMalloc(10,xString*);
   g_option.head_file   = xMalloc(10,xString*);

   g_option.directory[0] = string_new_init("./");  // default open current directory.
   
}


void options_add_file(char* file)
{
   if ( g_option.head_num % opt_incraese > 7 )
      g_option.head_file = xRealloc(g_option.head_file, (g_option.head_num + opt_incraese),xString*);

  g_option.head_file[g_option.head_num++] = string_new_init(file);
}


void options_add_dir(char* dir)
{
   if ( g_option.dir_num % opt_incraese > 7 )
      g_option.directory = xRealloc(g_option.directory, (g_option.dir_num + opt_incraese),xString*);

   g_option.directory[g_option.dir_num++] = string_new_init(dir);
   
}


void options_store_bin_file()
{
   FILE * file_ptr;    
   long   file_size , result;     

   file_ptr = fopen (g_option.bin_file->buffer, "rb" );   

   if (file_ptr==NULL)    {       
      fputs ("Open Raw Data File error. \n",stderr);        
      exit (1);   
   }   

   fseek (file_ptr , 0 , SEEK_END);    
   file_size = ftell (file_ptr);    
   rewind (file_ptr);    

   g_bin_buff = xMalloc(file_size, char);   
   
   if (g_bin_buff == NULL)   {        
      fputs ("Memory error.\n",stderr);        
      exit (2);    
   }   

   result = fread (g_bin_buff,1,file_size,file_ptr);  
   
   if (result != file_size)    {       
      fputs ("Reading Raw Data File error.\n",stderr);        
      exit (3);    
   }    

   fclose (file_ptr); 
   
}




extern void   options_process(int argc, char** argv)
{
   int ch = 0 ;
   
   opterr = 0 ;

   options_init();
   
   while( (ch = getopt(argc, argv, "i:s:b:rf:")) != -1){
      
      switch (ch){
         case 's': 
            string_copy(g_option.struct_name, optarg); 
            break;

         case 'b': 
            string_copy(g_option.bin_file,    optarg); 
            break;

         case 'f': 
            options_add_file(optarg);                  
            break;

         case 'i': 
            options_add_dir (optarg);                  
            break;    

         case 'r': 
            g_option.recurse = TRUE;   
            break;

         case 'h': 
         default :  
            goto end; 
            break;
       }
   }
   
   if (( get_string_length(g_option.struct_name) == 0 ) || 
	( get_string_length(g_option.bin_file) == 0))
	  goto end;

   return;
   
end:
   options_print_help(); 
}

