target remote :6666
symbol-file /home/bot/code/esp-idf/debuggwithjlink\build\debuggwithjlink.elf
mon reset halt
flushregs
thb app_main
c