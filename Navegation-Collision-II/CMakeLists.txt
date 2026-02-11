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

include(FetchContent)
FetchContent_Declare(
  rvo2
  GIT_REPOSITORY https://github.com/snape/RVO2.git
  GIT_TAG        main 
)

set(BUILD_TESTING OFF)
FetchContent_MakeAvailable(rvo2)

FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG        v0.13.1       # Or a specific tag like v0.11.1
    GIT_SHALLOW    TRUE         # Speeds up download
)

FetchContent_MakeAvailable(tracy)

option(TRACY_ENABLE "Enable Tracy Profiler" ON)
if(TRACY_ENABLE)
  add_compile_definitions(TRACY_ENABLE)
endif()

set(BUILD_DEMO OFF)
add_subdirectory(vendor/Engine)

file(GLOB_RECURSE DEMO_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

message("Src: ${DEMO_SOURCES}")


add_executable(${name} ${DEMO_SOURCES})
target_link_libraries(${name} PRIVATE Engine RVO)
target_include_directories(${name} PRIVATE "${CMAKE_SOURCE_DIR}/include")

if(TRACY_ENABLE)
    target_link_libraries(${name} PRIVATE TracyClient) 
    if(TARGET imgui_lib)
        target_link_libraries(imgui_lib PRIVATE TracyClient)
    endif()
endif()

target_compile_definitions(${name} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)

# add_custom_target(ApplicationScripts ALL
#     COMMAND ${CMAKE_COMMAND} -E copy
#             "${CMAKE_SOURCE_DIR}/graph.py"
#             "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/graph.py"         # Correct: Copy to executable's folder
# )
#
#
