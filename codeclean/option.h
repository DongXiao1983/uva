#ifndef _OPETION_H_
#define _OPETION_H_

struct option_st {

    unsigned int  recurse ;
    unsigned int  dir_num ;
    unsigned int  head_num; 
    
    xString*   bin_file     ;
    xString*   struct_name  ;  
    
    xString**  directory   ;  
    xString**  head_file   ;
    
};

struct option_description_st{
   const char* description;
};


void options_print_identification (void);

void options_print_description(void);

void options_add_file(char* file);

void options_add_dir(char* dir);

void options_store_bin_file();

void options_print_help();

extern void  options_process(int argc, char** argv);


#endif

