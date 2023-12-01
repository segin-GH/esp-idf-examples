#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void app_main()
{
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime;

    // Take a snapshot of the number of tasks in case it changes while this function is executing.
    uxArraySize = uxTaskGetNumberOfTasks();

    // Allocate an array of TaskStatus_t structures.
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL)
    {
        // Generate the (binary) data.
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

        // Print the header
        printf("| %-20s | %-10s | %-22s |\n", "Task Name", "Priority", "Stack High Water Mark");
        printf("|----------------------|------------|------------------------|\n");

        // For each task, print out the task name, priority, and stack high water mark in a tabular format
        for (x = 0; x < uxArraySize; x++)
        {
            printf("| %-20s | %-10d | %-22d |\n",
                   pxTaskStatusArray[x].pcTaskName,
                   pxTaskStatusArray[x].uxCurrentPriority,
                   pxTaskStatusArray[x].usStackHighWaterMark);
        }

        // The array is no longer needed, free the memory it consumes.
        vPortFree(pxTaskStatusArray);
    }
}