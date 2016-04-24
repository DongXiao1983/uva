#define START   2
#define SIZE    START*5
#define LENGTH  1 << 4
#define WIDE    SIZE+1
#define HIGH    14-4
#define DEEP    10/START
#define EXP     ((((1+START)*3 -1) >>START )|3)

struct MutilMarco{
	int size[SIZE];
	char length[LENGTH];
	int  wide[WIDE];
        char high[HIGH];
	char deep[DEEP];
	int  exp[EXP];
};
