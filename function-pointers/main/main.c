#include <stdio.h>
#include <string.h>

typedef struct struct_person 
{
    char firstName[20];
    char lastName[20];
    int age;
}Person;

void updatePerson(Person *person1);

void app_main(void)
{
    Person person1;
    updatePerson(&person1);
    printf("person: %s %s is %d years old \n",person1.firstName,
            person1.lastName,person1.age);

}

void updatePerson(Person *person1)
{
    strcpy(person1->firstName,"bob");
    strcpy(person1->lastName,"fisher");
    person1->age = 35;
}