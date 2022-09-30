#include <stdio.h>
#include <string.h>

typedef struct struct_person 
{
    char firstName[20];
    char lastName[20];
    int age;
}Person;

void updatePerson(Person *person1);
void addecxlam(char *word);

void app_main(void)
{
    Person person1;
    updatePerson(&person1);
    char word[20] = {"hello World"};
    addecxlam(word);
    printf("%s\n",word);
    printf("person: %s %s is %d years old \n",person1.firstName,
            person1.lastName,person1.age);

}


void addecxlam(char *word)
{
    const char ex[] = "!";
    strcat(word,ex);
}

void updatePerson(Person *person1)
{
    strcpy(person1->firstName,"bob");
    strcpy(person1->lastName,"fisher");
    person1->age = 35;
}