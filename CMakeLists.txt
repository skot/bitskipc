cmake_minimum_required(VERSION 3.0)
project(bitskipc)

# Add the libftdi library
add_subdirectory(lib/libftdi)

# Add the source files
add_executable(bitskipc bitskipc.c serial_monitor.c bm1366.c pretty.c crc.c)

# Include the ftdi.h header file
target_include_directories(bitskipc PRIVATE lib/libftdi/src)

# Link the libftdi library to the executable
target_link_libraries(bitskipc ftdi1)