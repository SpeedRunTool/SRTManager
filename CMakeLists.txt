cmake_minimum_required(VERSION 3.25)
set(CMAKE_C_COMPILER "C:/msys64/clang64/bin/clang.exe" CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER "C:/msys64/clang64/bin/clang++.exe" CACHE FILEPATH "" FORCE)
project (SRTManager)

function(set_from_env var_name default_value var_type var_desc)
  if(DEFINED ENV{${var_name}})
    set(${var_name} $ENV{${var_name}} CACHE ${var_type} ${var_desc} FORCE)
  else()
    set(${var_name} ${default_value} CACHE ${var_type} ${var_desc} FORCE)
  endif()
endfunction()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".dll")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
set(CMAKE_EXECUTABLE_ENABLE_EXPORTS TRUE)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if(BUILD_SHARED_LIBS)
    add_definitions(-DBUILD_SHARED_LIBS)
else()
    add_definitions(-DBUILD_STATIC_LIBS)
endif()

include(CTest)
enable_testing()
include(GoogleTest)
find_package(GTest CONFIG REQUIRED)

#set(CMAKE_C_STANDARD 17) # 90, 99, 11, 17, 23
#set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 23)  # 98, 11, 14, 17, 20, 23, 26
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
string(ASCII 169 COPYRIGHT)

# VERSIONINFO properties (Ref: https://learn.microsoft.com/en-us/windows/win32/menurc/versioninfo-resource)
set(RC_PRODUCTNAME "SRT Manager")
set(RC_FILEDESCRIPTION "A program for installing, uninstalling, and updating SRT Host and its plugins.")
set(RC_COMPANYNAME "Travis J. Gutjahr")
set(RC_LEGALCOPYRIGHT "Copyright ${COPYRIGHT} 2024 Travis J. Gutjahr")
set(RC_INTERNALNAME "${PROJECT_NAME}")
set(RC_ORIGINALFILENAME "${RC_INTERNALNAME}.exe")

# Major.Minor.Patch.Rev
set_from_env(RC_VERSION_MAJOR 0 STRING "Version (major) of the product.")
set_from_env(RC_VERSION_MINOR 1 STRING "Version (minor) of the product.")
set_from_env(RC_VERSION_PATCH 0 STRING "Version (patch) of the product.")
set_from_env(RC_VERSION_BUILD 1 STRING "Version (build) of the product.") # Auto-increment in build CI/CD.

# Compile the version information.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/VersionInfo.rc.in ${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.rc @ONLY)

add_definitions("-DSRTMGR_VERSION_MAJOR=${RC_VERSION_MAJOR}")
add_definitions("-DSRTMGR_VERSION_MINOR=${RC_VERSION_MINOR}")
add_definitions("-DSRTMGR_VERSION_PATCH=${RC_VERSION_PATCH}")
add_definitions("-DSRTMGR_VERSION_BUILD=${RC_VERSION_BUILD}")

include(FetchContent)
set(FETCHCONTENT_QUIET off)

FetchContent_Declare(
    SHA-2
    GIT_REPOSITORY https://github.com/amosnier/sha-2.git
    GIT_TAG        b29613850d6e54e7159197ef42c7d22d012b6367 # origin/master
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/extern/amosnier/sha-2
)
FetchContent_MakeAvailable(SHA-2)

# vcpkg dependencies.
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Required libraries we'll link for ImGUI.
find_library(D3D11_LIBRARY d3d11 REQUIRED PATHS C:/msys64/clang64/lib)
find_library(D3DCOMPILER_LIBRARY d3dcompiler REQUIRED PATHS C:/msys64/clang64/lib)

include(CMakePrintHelpers)
cmake_print_variables(
    D3D11_LIBRARY
    D3DCOMPILER_LIBRARY
)

add_compile_options(
    -Wall -Wextra -Wpedantic
    -m64 -masm=intel
    $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>,$<CONFIG:RELWITHDEBINFO>>:-O3> # Optimize.
    #$<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:-s> # Debug symbol stripping. [Requires GCC]
    $<$<OR:$<CONFIG:RELWITHDEBINFO>,$<CONFIG:DEBUG>>:-g> # Set symbol type.
    #$<$<OR:$<CONFIG:RELWITHDEBINFO>,$<CONFIG:DEBUG>>:-gcodeview> # Set symbol type. (MSVC++ format) [Requires GCC 14+ or clang]
    $<$<CONFIG:DEBUG>:-O0> # No optimizations.
    )
add_compile_definitions(
    $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>,$<CONFIG:RELWITHDEBINFO>>:RELEASE>
    $<$<CONFIG:DEBUG>:DEBUG>
    )
add_link_options(
    -static -stdlib=libc++ -lc++ -lc++abi
)

cmake_print_variables(
    CMAKE_C_COMPILER
    CMAKE_CXX_COMPILER
)

target_link_libraries(imgui::imgui INTERFACE ${D3D11_LIBRARY} ${D3DCOMPILER_LIBRARY})
include_directories(include extern/amosnier/sha-2)

### Library
add_library(${PROJECT_NAME}
${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.rc
extern/amosnier/sha-2/sha-256.c
src/Helpers.cpp
src/UI.cpp
)
target_link_libraries(${PROJECT_NAME}
PRIVATE imgui::imgui
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD DEPENDS ${PROJECT_NAME} COMMAND $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:${CMAKE_STRIP}> ARGS --strip-all ${PROJECT_NAME}.dll)

### Executable
add_executable(${PROJECT_NAME}App
${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.rc
src/Main.cpp
)
target_link_libraries(${PROJECT_NAME}App
PRIVATE ${PROJECT_NAME}
PRIVATE SDL2::SDL2main
PRIVATE SDL2::SDL2
)
set_target_properties(${PROJECT_NAME}App
PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME}App POST_BUILD DEPENDS ${PROJECT_NAME}App COMMAND $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:${CMAKE_STRIP}> ARGS --strip-all ${PROJECT_NAME}.exe)

### Unit Tests
include_directories(${GTest_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}Tests
tests/test_Strings.cpp
)
target_link_libraries(${PROJECT_NAME}Tests
PRIVATE GTest::gtest
PRIVATE GTest::gtest_main
PRIVATE ${PROJECT_NAME}
)

gtest_discover_tests(${PROJECT_NAME}Tests
PROPERTIES
DISCOVERY_TIMEOUT 30
TEST_LIST discovered_tests
)
