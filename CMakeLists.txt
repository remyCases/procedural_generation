cmake_minimum_required(VERSION "3.15")

project(${CMAKE_PROJECT_NAME})

if(UNIX AND NOT APPLE)
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG OFF)
    list(PREPEND CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/pkgconfig)
    list(FILTER CMAKE_PREFIX_PATH EXCLUDE REGEX "/mnt/")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Generator is set to ${CMAKE_GENERATOR}")

enable_language(C CXX)

set(CMAKE_C_STANDARD "99")
set(CMAKE_CXX_STANDARD "23")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM MATCHES Windows)
	message(STATUS "Target system is windows")
else(CMAKE_SYSTEM MATCHES Linux)
	message(STATUS "Target system is linux")
endif()

set_source_files_properties(
	"stb/source/*.c"
	"stb/include/*.h"
    PROPERTIES
    COMPILE_FLAGS "-w" 
)

if(MSVC)
    # MSVC-specific flags
    add_compile_options(
        /W4
        /MP
        /Zi
    )
else()
    # GCC/Clang flags
    add_compile_options(
        -Wall
        -Wextra
		-O2
        -pthread
		-fstack-protector
        -Wformat=2
        -Wpedantic
        -Wstrict-prototypes
        -Wshadow
        -Wuninitialized
    )

    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    if(MSVC)
        add_compile_options(/Od /RTC1)  # Disable optimization, enable runtime checks
    else()
        add_compile_options(-O0 -g3)    # No optimization, max debug info
    endif()
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(PNG REQUIRED libpng)
find_package(OpenGL REQUIRED)

file(GLOB MAIN_SRCS
    "main.c"
	"stb/include/*.h"
	"stb/source/*.c"
	"include/*.h"
    "source/*.c"
)

file(GLOB TEST_SRCS
    "test.c"
	"stb/include/*.h"
	"stb/source/*.c"
	"include/*.h"
    "source/*.c"
)

add_executable(${CMAKE_PROJECT_NAME} ${MAIN_SRCS})
target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${PNG_LIBRARIES}
        m
)
target_include_directories(${CMAKE_PROJECT_NAME}
    SYSTEM PRIVATE
        ${OPENGL_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${PNG_INCLUDE_DIRS}
)

add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SRCS})
target_link_libraries(${CMAKE_PROJECT_NAME}_test
    PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${PNG_LIBRARIES}
        m
)
target_include_directories(${CMAKE_PROJECT_NAME}_test
    SYSTEM PRIVATE
        ${OPENGL_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${PNG_INCLUDE_DIRS}
)