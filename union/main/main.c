#include <stdio.h>

typedef union
{
    uint32_t num;
    uint8_t bytes[2];
}Cas_uid;

Cas_uid uid;

void app_main()
{
    uid.num = 1000;
    printf("%i \n", uid.bytes[0]);
    printf("%i \n", uid.bytes[1]);

    uid.bytes[0] = 232;
    uid.bytes[1] = 3;
    printf("%i \n", uid.num);

}
