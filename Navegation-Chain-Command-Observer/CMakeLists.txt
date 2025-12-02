# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Project name and language
project(navegation LANGUAGES CXX C)
set(name ${PROJECT_NAME})

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(BUILD_DEMO OFF)
add_subdirectory(vendor/Engine)

file(GLOB_RECURSE DEMO_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

message("Src: ${DEMO_SOURCES}")

# set(DEMO_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
add_executable(${name} ${DEMO_SOURCES})
target_link_libraries(${name} PRIVATE Engine)
target_include_directories(${name} PRIVATE "${CMAKE_SOURCE_DIR}/include")

# add_custom_target(ApplicationScripts ALL
#     COMMAND ${CMAKE_COMMAND} -E copy
#             "${CMAKE_SOURCE_DIR}/graph.py"
#             "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/graph.py"         # Correct: Copy to executable's folder
# )
#
#
