#include <stdio.h>
#include <string.h>

struct Person 
{
    char firstName[20];
    char lastName[20];
    int age;
};


void app_main(void)
{
    struct Person person1;
    strcpy(person1.firstName,"bob");
    strcpy(person1.lastName,"fisher");
    person1.age = 35;
    printf("person: %s %s is %d years old \n",person1.firstName,
            person1.lastName,person1.age);

}
