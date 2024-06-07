#include "hspl_rpc.h"
#include <stdio.h>

extern "C" void app_main(void)
{
    Rpc rpc;
    rpc.my_cpp_func();
}
