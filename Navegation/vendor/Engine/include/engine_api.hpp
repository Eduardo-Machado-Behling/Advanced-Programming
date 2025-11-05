#ifndef ENGINE_API_HPP
#define ENGINE_API_HPP

// clang-format off
// Check for Windows (as __declspec is a Microsoft extension)
#if defined(_WIN32) || defined(_WIN64)
    #ifdef BUILDING_ENGINE_DLL // This macro is defined when building the DLL
        #define ENGINE_API __declspec(dllexport)
    #else // This is defined when a client includes the header
        #define ENGINE_API __declspec(dllimport)
    #endif
#else
    // For other platforms like Linux/macOS, GCC/Clang use visibility attributes
    #define ENGINE_API __attribute__((visibility("default")))
#endif

// clang-format on
#endif // ENGINE_API_HPP
