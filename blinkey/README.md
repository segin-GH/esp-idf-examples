# Blinking an LED with ESP-IDF

This code is written in C and uses the ESP-IDF framework to blink an LED connected to the ESP32 development board. It uses the `driver/gpio.h` header file to control the GPIO (General Purpose Input/Output) pins on the ESP32, which allows the code to turn the LED on and off.

The `app_main()` function is the entry point of the program, and it is executed when the ESP32 boots up. Inside this function, the LED pin is selected and its direction is set to output using the `gpio_pad_select_gpio()` and `gpio_set_direction()` functions.

Next, the code enters an infinite loop where it alternates between turning the LED on and off. The `gpio_set_level()` function is used to set the level (i.e. the voltage) of the LED pin to either high (1) or low (0), which corresponds to turning the LED on or off. The `vTaskDelay()` function is used to add a delay between each iteration of the loop, which controls the blinking frequency of the LED.

Additionally, the code also prints a message to the console using the `printf()` function each time the LED is turned on or off. This allows the user to see the status of the LED on the console.
