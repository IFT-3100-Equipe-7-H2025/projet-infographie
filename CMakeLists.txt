cmake_minimum_required(VERSION 3.30)
project(infographie VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/context/GLFWContext.cpp
        src/context/GLFWContext.h
        src/shader.h
        src/opengl_include.h
)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        src
)

# checks if x11 is the display server, used to disable wayland
if (LINUX)
    execute_process(
        COMMAND bash -c "loginctl show-session $(loginctl | grep $(whoami) | awk '{print $1}') -p Type"
        OUTPUT_VARIABLE result_display_server
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if ("${result_display_server}" STREQUAL "Type=x11")
        set(display_server_x11 TRUE)
    else()
        set(display_server_x11 FALSE)
    endif()
endif()


if (MSVC)
    # Set the app as solution startup project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    # Parallel c++ compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif ()

# Dependencies ========================================================================================
include(cmake/CPM.cmake)

# OpenGL ==============================================================================================
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
# =====================================================================================================

# glfw ================================================================================================
if (NOT TARGET glfw)
    if (display_server_x11)
        CPMAddPackage(
                NAME glfw
                GITHUB_REPOSITORY glfw/glfw
                GIT_TAG 3.4
                OPTIONS "GLFW_BUILD_DOCS OFF" "GLFW_BUILD_TESTS OFF" "GLFW_BUILD_EXAMPLES OFF" "GLFW_BUILD_WAYLAND OFF"
        )
    else()
        CPMAddPackage(
                NAME glfw
                GITHUB_REPOSITORY glfw/glfw
                GIT_TAG 3.4
                OPTIONS "GLFW_BUILD_DOCS OFF" "GLFW_BUILD_TESTS OFF" "GLFW_BUILD_EXAMPLES OFF"
        )
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
# =====================================================================================================

# glew ================================================================================================
if (NOT TARGET libglew_static)
    CPMAddPackage(
            NAME "GLEW"
            GITHUB_REPOSITORY "Perlmint/glew-cmake"
            GIT_TAG "glew-cmake-2.2.0"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)
# =====================================================================================================

# Compile definitions =================================================================================
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __RELEASE__ PRIVATE __NDEBUG__)
endif ()
# =====================================================================================================
