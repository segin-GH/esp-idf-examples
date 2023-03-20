# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/bot/esp/esp-idf/components/bootloader/subproject"
  "/home/bot/code/esp-idf/TaskNotification-2/build/bootloader"
  "/home/bot/code/esp-idf/TaskNotification-2/build/bootloader-prefix"
  "/home/bot/code/esp-idf/TaskNotification-2/build/bootloader-prefix/tmp"
  "/home/bot/code/esp-idf/TaskNotification-2/build/bootloader-prefix/src/bootloader-stamp"
  "/home/bot/code/esp-idf/TaskNotification-2/build/bootloader-prefix/src"
  "/home/bot/code/esp-idf/TaskNotification-2/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/bot/code/esp-idf/TaskNotification-2/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
