
#ifndef _EMPLOYEE_
#define _EMPLOYEE_

/* Hometown */
typedef enum Home
{
    Anhui = 1,
    Beijing = 2, 
    Zhejiang = 33
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

#endif
