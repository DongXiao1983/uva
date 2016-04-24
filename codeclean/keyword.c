#include "keyword.h"
#include <string.h>
#include "memory.h"

#define HASH_EXPONENT 7  

typedef struct sHashEntry {
	struct sHashEntry *next;
	const  char *string;
	int    value;
} hash_entry;

static const unsigned int gTable_size = 1 << HASH_EXPONENT;
static hash_entry **gHashTable = NULL;


static hash_entry **__get_hash_table (void)
{
	static boolean allocated = FALSE;

	if (! allocated)
	{
		unsigned int i;

		gHashTable = xMalloc (gTable_size, hash_entry*);

		for (i = 0  ;  i < gTable_size  ;  ++i)
			gHashTable [i] = NULL;

		allocated = TRUE;
	}
	return gHashTable;
}

static hash_entry *__get_hash_table_entry (unsigned long hashedValue)
{
	hash_entry **const table = __get_hash_table ();
	hash_entry *entry;

	entry = table [hashedValue];

	return entry;
}

static unsigned long __get_hash_value (const char *const string)
{
	unsigned long value = 0;
	const unsigned char *p;

	for (p = (const unsigned char *) string  ;  *p != '\0'  ;  ++p)
	{
		value <<= 1;
		if (value & 0x00000100L)
			value = (value & 0x000000ffL) + 1L;
		value ^= *p;
	}

	value *= 40503L;              
	value &= 0x0000ffffL;         
	value >>= 16 - HASH_EXPONENT;  

	return value;
}

static hash_entry *__new_table_entry (
		const char *const string,  int value)
{
	hash_entry *const entry = xMalloc (1, hash_entry);

	entry->next     = NULL;
	entry->string   = string;
	entry->value    = value;

	return entry;
}


extern void keyword_add (const char *const string,  int value)
{
	const unsigned long hashedValue = __get_hash_value (string);
	hash_entry *entry = __get_hash_table_entry (hashedValue);

	if (entry == NULL)
	{
		hash_entry **const table = __get_hash_table ();
		table [hashedValue] = __new_table_entry (string, value);
	}
	else
	{
		hash_entry *prev = NULL;

		while (entry != NULL)
		{
			prev = entry;
			entry = entry->next;
		}
		if (entry == NULL)
		{
			prev->next = __new_table_entry (string, value);
		}
	}
}

extern int keyword_lookup (const char *const string)
{
	const unsigned long hashedValue = __get_hash_value (string);
	hash_entry *entry = __get_hash_table_entry (hashedValue);
	int result = -1;

	while (entry != NULL)
	{
		if (  strcmp (string, entry->string) == 0)
		{
			result = entry->value;
			break;
		}
		entry = entry->next;
	}
	return result;
}

extern void keyword_table_free (void)
{
	if (gHashTable != NULL)
	{
		unsigned int i;

		for (i = 0  ;  i < gTable_size  ;  ++i)
		{
			hash_entry *entry = gHashTable [i];

			while (entry != NULL)
			{
				hash_entry *next = entry->next;
				xfree (entry);
				entry = next;
			}
		}
		xfree (gHashTable);
	}
}


static void printEntry (const hash_entry *const entry)
{
	printf ("  %-15s\r\n", entry->string);
}

static unsigned int printBucket (const unsigned int i)
{
	hash_entry **const table = __get_hash_table ();
	hash_entry *entry = table [i];
	unsigned int measure = 1;
	boolean first = TRUE;

	printf ("%2d:", i);
	if (entry == NULL)
		printf ("\r\n");
	else while (entry != NULL)
	{
		if (! first)
			printf ("    ");
		else
		{
			printf (" ");
			first = FALSE;
		}
		printEntry (entry);
		entry = entry->next;
		measure = 2 * measure;
	}
	return measure - 1;
}

extern void printKeywordTable (void)
{
	unsigned long emptyBucketCount = 0;
	unsigned long measure = 0;
	unsigned int i;

	for (i = 0  ;  i < gTable_size;  ++i)
	{
		const unsigned int pass = printBucket (i);

		measure += pass;
		if (pass == 0)
			++emptyBucketCount;
	}

	printf ("spread measure = %ld\r\n", measure);
	printf ("%ld empty buckets\r\n", emptyBucketCount);
}


