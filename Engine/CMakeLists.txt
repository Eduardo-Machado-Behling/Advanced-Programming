# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Project name and language
project(Engine LANGUAGES CXX C)
set(name ${PROJECT_NAME})

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# Specify the public include directories for the 'glad' library.
# The "PUBLIC" keyword means that any other target that links against 'glad'
# will automatically get this include path added.


# --- Executable ---

# Set the output directory for the executable to './bin/'
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE EXECUTABLE_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${name} SHARED ${EXECUTABLE_SOURCES})

set_target_properties(${name} PROPERTIES
	COMPILE_FLAGS_RELEASE "-O3"
)

target_link_libraries(${name} PUBLIC glfw glad)

target_compile_definitions(${name} PUBLIC GLFW_INCLUDE_NONE)

target_include_directories(${name} PUBLIC
	"${CMAKE_SOURCE_DIR}/include"               # General include directory (e.g., for GLAD headers)
)

# On Unix-like systems (excluding macOS), link against specific OpenGL and threading libraries.
if(UNIX AND NOT APPLE)
	target_link_libraries(${name} PUBLIC
		-lGL      # Standard OpenGL library
		-ldl      # Dynamic linking library
		-lpthread # POSIX threads library
	)
endif()
