cmake_minimum_required(VERSION 3.30)
project(infographie VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/opengl/context/GLFWContext.cpp
        src/opengl/context/GLFWContext.h
        src/opengl/context/IMGUIContext.h
        src/opengl/context/IMGUIContext.cpp
        src/opengl/opengl_include.h
        src/opengl/Macros.h
        src/opengl/rendering/Shader.cpp
        src/opengl/rendering/Shader.h
        src/opengl/rendering/Renderer.cpp
        src/opengl/rendering/Renderer.h
        src/opengl/rendering/RendererId.h
        src/opengl/rendering/VertexArray.cpp
        src/opengl/rendering/VertexArray.h
        src/opengl/rendering/VertexBuffer.cpp
        src/opengl/rendering/VertexBuffer.h
        src/opengl/rendering/IndexBuffer.cpp
        src/opengl/rendering/IndexBuffer.h
        src/opengl/rendering/VertexArrayObject.cpp
        src/opengl/rendering/VertexArrayObject.h
        src/opengl/rendering/VertexBufferLayout.h
        src/opengl/rendering/Texture.cpp
        src/opengl/rendering/Texture.h
        src/debug-trap.h
        src/stb_image.cpp
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

# glm =================================================================================================
if (NOT TARGET glm)
    CPMAddPackage(
            NAME glm
            GITHUB_REPOSITORY g-truc/glm
            GIT_TAG "1.0.1"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
# =====================================================================================================

# stb_image ===========================================================================================
if (NOT TARGET stb_image)
    CPMAddPackage(
            NAME stb_image
            GITHUB_REPOSITORY nothings/stb
            GIT_TAG master
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${stb_image_SOURCE_DIR})
# =====================================================================================================
#
# imgui ===============================================================================================
if (NOT TARGET imgui)
    CPMAddPackage(
                NAME imgui
                GITHUB_REPOSITORY ocornut/imgui
                GIT_TAG docking
        )
endif()

# Add ImGui source files
target_sources(${PROJECT_NAME}
        PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR})
# =====================================================================================================

# imgui file explorer =================================================================================
if (NOT TARGET imgui_filedialog)
    CPMAddPackage(
            NAME imgui_filedialog
            GITHUB_REPOSITORY AirGuanZ/imgui-filebrowser
            GIT_TAG master
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_filedialog_SOURCE_DIR})
# =====================================================================================================
#
# Compile definitions =================================================================================
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __DEBUG__)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE __RELEASE__ PRIVATE __NDEBUG__)
endif ()
# =====================================================================================================

# Symlink resources folder into build directory =======================================================
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/res
        COMMENT "Creating symlink to res folder"
)
# =====================================================================================================
