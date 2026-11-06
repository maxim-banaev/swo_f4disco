cmake_minimum_required(VERSION 3.22)

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi.cmake)
endif()

project(swo_f4disco LANGUAGES C)

set(CMAKE_C_STANDARD 11)

add_executable(swo_f4disco
    src/startup.c
    src/swo.c
    src/main.c
    src/syscalls.c
)
set_target_properties(swo_f4disco PROPERTIES SUFFIX ".elf")

target_compile_options(swo_f4disco PRIVATE -Wall -Wextra -ffunction-sections -fdata-sections -g3)
target_link_options(swo_f4disco PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VG.ld
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/swo_f4disco.map
    -Wl,--print-memory-usage
    -Wl,--no-warn-rwx-segments
)
set_property(TARGET swo_f4disco APPEND PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VG.ld)

add_custom_command(TARGET swo_f4disco POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:swo_f4disco>)
