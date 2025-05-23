cmake_minimum_required(VERSION 3.10)
project(vbtangOpenglPlayground)

# set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

# add GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rd/glfw)

# add glad
add_library(glad 3rd/glad/src/glad.c)
target_include_directories(glad PUBLIC 3rd/glad/include)

# add ImGui sources
set(IMGUI_SOURCES
    3rd/imgui/imgui.cpp
    3rd/imgui/imgui_demo.cpp
    3rd/imgui/imgui_draw.cpp
    3rd/imgui/imgui_tables.cpp
    3rd/imgui/imgui_widgets.cpp
    3rd/imgui/backends/imgui_impl_glfw.cpp
    3rd/imgui/backends/imgui_impl_opengl3.cpp
)

# add stb_image
set(STB_IMAGE stb_image.cpp)

# playgrounds
file(GLOB_RECURSE PLAYGROUNDS_SOURCES 
        "playgrounds/*.cpp"
        "playgrounds/*/*.cpp"
        "playgrounds/*/*/*.cpp"
)
set(PLAYGROUNDS
    playgrounds/VbtangOpenGLPlaygroundManager.cpp
    ${PLAYGROUNDS_SOURCES}
)
# create executable
add_executable(${PROJECT_NAME} main.cpp ${PLAYGROUNDS} ${IMGUI_SOURCES} ${STB_IMAGE})

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ./
    3rd/imgui
    3rd/imgui/backends
    3rd/stb_image
    3rd/glm
    playgrounds
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
)
