# include <iostream>
# include <stdio.h>
# include <freertos/FreeRTOS.h>
# include <freertos/task.h>

extern "C" {
    void app_main(void);
}

void app_main(void)
{
    while(true){
        std::cout << "heloWorld\n";
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
