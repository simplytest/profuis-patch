set(CMAKE_SYSTEM_NAME Windows)

add_compile_definitions(-DNTDDI_VERSION=0x0A000005)

set(CMAKE_C_COMPILER    i686-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER  i686-w64-mingw32-c++)

set(CMAKE_CXX_FLAGS      "-static")
set(CMAKE_FIND_ROOT_PATH "/usr/i686-w64-mingw32/")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
