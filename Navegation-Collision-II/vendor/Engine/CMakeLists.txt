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

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot
    GIT_TAG        master
)
FetchContent_MakeAvailable(implot)

# 2. Create a specific library for ImGui + The Backends you need (GLFW + OpenGL3)
# We assume you are using GLAD and GLFW based on your previous messages.
add_library(imgui_lib STATIC
    # Core
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    
    # Backends (The "Bridge" between Engine and ImGui)
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

	#implot
	${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
    ${implot_SOURCE_DIR}/implot_demo.cpp
)

target_include_directories(imgui_lib PUBLIC 
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
	${implot_SOURCE_DIR}
)

add_subdirectory(vendor/glad)

# For shared object Engine
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(imgui_lib PUBLIC glfw glad)
set_target_properties(imgui_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)



# Set the output directory for the executable to './bin/'
file(GLOB_RECURSE EXECUTABLE_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
)
add_library(${name} SHARED ${EXECUTABLE_SOURCES})

target_link_libraries(${name} PUBLIC glfw glad imgui_lib)
target_compile_definitions(${name} PUBLIC GLFW_INCLUDE_NONE)
target_compile_definitions(${name} PUBLIC BUILDING_ENGINE_DLL)
target_include_directories(${name} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"               
)
target_compile_definitions(${name} PRIVATE 
    # We wrap the path in escaped quotes: \"...\"
    RUNTIME_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
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
