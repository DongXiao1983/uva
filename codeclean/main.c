#include "common.h"

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "memory.h"
#include "file.h"
#include "parse.h"
#include "chead.h"
#include "keyword.h"
#include "entry.h"
#include "option.h"


extern struct option_st g_option;

static boolean recurse_opendir (const char *const dirName);

static boolean recurse_into_directory (const char *const dirName)
{
	boolean resize = FALSE;
	if (is_recursive_link (dirName))
		;//printf("ignoring \"%s\" (recursive link)\n", dirName);
	else if (! g_option.recurse)
		;//printf("ignoring \"%s\" (directory)\n", dirName);
	else
	{
		;//printf("RECURSING into directory \"%s\"\n", dirName);
		resize = recurse_opendir (dirName);
	}
	return resize;
}

static boolean create_tags_for_file (const char *const filename)
{
	boolean resize = FALSE;
	fileStatus *status = file_stat (filename);

	if (status->isSymbolicLink )
		;//printf ("ignoring \"%s\" (symbolic link)\n", filename);
	else if (! status->exists)
		;//error (WARNING | ERROR, "cannot open source file \"%s\"", filename);
	else if (status->isDirectory)
		resize = recurse_into_directory (filename);
	else if (! status->isNormalFile)
		;//printf ("ignoring \"%s\" (special file)\n", filename);
	else
		resize = parse_file (filename);

	file_stat_free (status);
	return resize;
}

static boolean recurse_opendir (const char *const dirName)
{
	boolean resize = FALSE;
	DIR *const dir = opendir (dirName);
	if (dir == NULL)
		error (WARNING | ERROR, "cannot recurse into directory \"%s\"", dirName);
	else
	{
		struct dirent *entry;
		while ((entry = readdir (dir)) != NULL)
		{
			if (strcmp (entry->d_name, ".") != 0  &&
				strcmp (entry->d_name, "..") != 0)
			{
				xString *filePath;
				if (strcmp (dirName, ".") == 0)
					filePath = string_new_init(entry->d_name);
				else
					filePath = combine_path_and_file (dirName, entry->d_name);
				resize |= create_tags_for_file (get_string_value(filePath));
				string_delete(filePath);
			}
		}
		closedir (dir);
	}
	return resize;
}


void pre_process_dirs()
{
   xString **dir = g_option.directory;
   while(*dir != NULL ){
      recurse_opendir((*dir)->buffer);
      dir++;
   }
}

void pre_process_files()
{
   xString **file = g_option.head_file;
   while(*file != NULL ){
      parse_file((*file)->buffer);
      file++;
   }
}

void pre_process_head_file()
{
   pre_process_dirs();

   pre_process_files();
}



int main(int argc , char** argv)
{
 
   
   set_current_directory();
   options_process(argc,argv);
   build_keyword_hashtable(); 
   entry_list_init();
   pre_process_head_file();
   options_store_bin_file();
   entry_list_sizeof_entry();
   //entry_list_print();
   entry_map_to_rawdata(g_option.struct_name);
  
	return 0;
}
