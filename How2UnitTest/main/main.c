#include <stdio.h>
#include <unity.h>

void app_main(void)
{
    UNITY_BEGIN();
        unity_run_test_by_name("sprintf");
    UNITY_END();


}

    TEST_CASE("sprintf", "[NoFormatOperations]")
    {
        char output[5];

        TEST_ASSERT_EQUAL(4, sprintf(output, "hey"));
        TEST_ASSERT_EQUAL_STRING("hey", output);
    }