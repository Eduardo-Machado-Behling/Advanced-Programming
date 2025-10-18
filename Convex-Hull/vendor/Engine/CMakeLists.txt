# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Project name and language
project(Engine LANGUAGES CXX C)
set(name ${PROJECT_NAME})

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_DEMO "Builds demo" ON)

# --- Dependencies ---
include(FetchContent)

# Optional: set a common prefix for all downloaded sources
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# Ensure we build these in Release mode, regardless of main project config
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

### GLFW
FetchContent_Declare(
		glfw
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		GIT_TAG        3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

add_subdirectory(vendor/glad)

# For shared object Engine
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)


# Set the output directory for the executable to './bin/'
file(GLOB_RECURSE EXECUTABLE_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
)
add_library(${name} SHARED ${EXECUTABLE_SOURCES})

target_link_libraries(${name} PUBLIC glfw glad)
target_compile_definitions(${name} PUBLIC GLFW_INCLUDE_NONE)
target_compile_definitions(${name} PUBLIC BUILDING_ENGINE_DLL)
target_include_directories(${name} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"               
)



# On Unix-like systems (excluding macOS), link against specific OpenGL and threading libraries.
if(UNIX AND NOT APPLE)
	target_link_libraries(${name} PUBLIC
		-lGL      # Standard OpenGL library
		-ldl      # Dynamic linking library
		-lpthread # POSIX threads library
	)
endif()

if(BUILD_DEMO)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
	file(GLOB_RECURSE DEMO_SOURCES
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	)
	add_executable(demo ${DEMO_SOURCES})
	target_link_libraries(demo PRIVATE ${name})

add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"  # Correct: Copy to executable's folder

    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/graph.py"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"         # Correct: Copy to executable's folder

    COMMENT "Copying assets and scripts..."
)
else()
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"  # Correct: Copy to executable's folder

    COMMENT "Copying assets and scripts..."
)
endif()

add_dependencies(CopyResources ${name})