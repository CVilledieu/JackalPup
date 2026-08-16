cmake_minimum_required(VERSION 3.20)
project(JackalPup)

find_package(SDL3 REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS volk shaderc_combinded)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)


# Create SDL library
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/#<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/#<CONFIGURATION>")
add_subdirectory(include/SDL EXCLUDE_FROM_ALL)



add_executable(JackalPup WIN32 )

# Link Libraries
target_link_libraries(JackalPup PRIVATE SDL3::SDL3)