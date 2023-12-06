#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

typedef struct
{
    void *data;
    size_t size;
    atomic_int refCount;
} GCItem;

#define NUMBER_OF_CONSUMING_TASKS 2

void app_main(void)
{
    GCItem item = {
        .data = (void *)malloc(11),
        .size = 0,
        .refCount = NUMBER_OF_CONSUMING_TASKS,
    };

    memset(item.data, 0, 11);
    printf("item.data: %p\n", item.data);
    memset(item.data, 'A', 10);
    printf("item.data: %s \n", (char *)item.data);

    if (atomic_fetch_sub(&item.refCount, 1) == 1)
    {
        free(item.data);
    }
}
