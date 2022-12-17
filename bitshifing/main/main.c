#include <stdio.h>

uint16_t memoryAddr = 0b0001;

void print_binary(unsigned int num) 
{
  int i;

  for (i = 3; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
  }
  printf("\n");
}


void app_main(void)
{
    for(int i = 0; i < 4; i++)
    {
        memoryAddr = memoryAddr << 1;
        print_binary(memoryAddr);
    }
}