1. readlog.pl 
=========================================================================
1.1 Usage:
    perl readlog.pl -s Employee -b Employee.bin
    perl readlog.pl -s Employee -b Employee.bin -i. -i../

1.2 Notes:
    readlog.pl will search current direcory as default.

    
    
2. How to get the binary files? (little endian as default)
=========================================================================
2.1 Struct definition
    /* Employee.h ---------------------------------------------------*/
    typedef enum Home
    {
        Zhejiang = 1,
        Shanghai = 2
    }Home;

    typedef struct Manager
    {
        int level;
    }Manager;

    typedef struct Engineer
    {
        int skills;
    }Engineer;

    typedef struct Person
    {
        char name[16];
        int  age;
        Home home;
    }Person;

    typedef union Position
    {
        Manager manager;
        Engineer engineer;
    }Position;

    typedef struct Employee
    {
        int  id;
        Person  person;
        Position position;
    }Employee;

2.2 Instance Initialization

    /* XXX.c --------------------------------------------------------*/
    Employee smith = { 0xABCDEFGH,  {"Fake Smith", 26, 1}, 1};

2.3 Raw data (byte array)

    raw_data [] = {0xAB, 0xCD, 0xEF, 0xGH....};

2.4 Hex View of Binary File (Vim / UltraEdit hex mode)
    +-----------------------------------------------------------------+
    |File: Emloyee.bin                                                |
    +-----------------------------------------------------------------+
    |00000000h: AB CD EF GH 46 61 6B 65 20 53 6D 69 74 68 00 00       |
    |00000010h: 00 00 00 00 1A 00 00 00 00 00 00 01 00 00 00 01       |
    |                                                                 |
    |                                                                 |
    |                                                                 |
    |                                                                 |
    |                                                                 |
    |                                                                 |
    +-----------------------------------------------------------------+




