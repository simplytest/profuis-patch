cmake_minimum_required(VERSION 3.25)
project(profui-patch LANGUAGES CXX VERSION 1.3)

# --------------------------------------------------------------------------------------------------------
# Setup Cross-Compilation on Linux
# --------------------------------------------------------------------------------------------------------

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  message(FATAL_ERROR "[${PROJECT_NAME}] Make sure to instruct cmake to use the toolchain file!")
endif()

# --------------------------------------------------------------------------------------------------------
# Create library
# --------------------------------------------------------------------------------------------------------

file(GLOB src "src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${src})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "MSIMG32")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF CXX_STANDARD_REQUIRED ON)

if (MINGW)
  target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--enable-stdcall-fixup")
endif()

if (NOT MSVC AND NOT IS_CI)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -pedantic -pedantic-errors -Wfatal-errors)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wno-cast-function-type -Wno-missing-field-initializers)
endif()

# --------------------------------------------------------------------------------------------------------
# Include directories
# --------------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME} PUBLIC "include")

# --------------------------------------------------------------------------------------------------------
# Link Dependencies and Add Sources
# --------------------------------------------------------------------------------------------------------

include("cmake/cpm.cmake")

CPMAddPackage(
  NAME           spdlog
  VERSION        1.17.0
  GIT_REPOSITORY "https://github.com/gabime/spdlog"
  OPTIONS        "SPDLOG_WCHAR_SUPPORT ON"
)

CPMAddPackage(
  NAME           lime
  VERSION        7.0.0
  GIT_REPOSITORY "https://github.com/Curve/lime"
  OPTIONS        "lime_entrypoint Platform"
)

CPMAddPackage(
  NAME           lockpp
  VERSION        3.2.0
  GIT_REPOSITORY "https://github.com/Curve/lockpp"
)

lime_mingw_generate_proxy(
    TARGET      "${PROJECT_NAME}"
    DEFINITIONS "${CMAKE_SOURCE_DIR}/exports.lime"
)

target_link_libraries(${PROJECT_NAME} PUBLIC cr::lime cr::lockpp spdlog::spdlog)
