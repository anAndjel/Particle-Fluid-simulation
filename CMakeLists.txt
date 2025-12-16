cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

# set(
#     CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O0 -g
#     -ggdb3 -DDEBUG -Wall -Wextra -Wshadow -Wpedantic
#     -Wno-variadic-macros -Wno-multistatement-macros
#     -Wstringop-overflow -Werror=return-type"
# )

project(particle_sim)

include_directories(fluidsim/extern)
include_directories(fluidsim/)
include_directories(fluidsim/extern/imgui)
include_directories(fluidsim/extern/implot)
include_directories(fluidsim/extern/glad)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)


add_executable(particle_sim
    fluidsim/main.cpp

    fluidsim/extern/imgui/imgui.cpp
    fluidsim/extern/imgui/imgui_demo.cpp
    fluidsim/extern/imgui/imgui_draw.cpp
    fluidsim/extern/imgui/imgui_tables.cpp
    fluidsim/extern/imgui/imgui_widgets.cpp
    fluidsim/extern/imgui/backends/imgui_impl_opengl3.cpp
    fluidsim/extern/imgui/backends/imgui_impl_glfw.cpp

    fluidsim/extern/implot/implot.cpp
    fluidsim/extern/implot/implot_demo.cpp
    fluidsim/extern/implot/implot_items.cpp

    fluidsim/extern/glad/glad.c


)

# Apply compile options to the target (instead of global flags)
target_compile_features(particle_sim PRIVATE cxx_std_20)
target_compile_options(particle_sim PRIVATE -O0 -g -ggdb3 -DDEBUG -Wall -Wextra -Wshadow -Wpedantic -Wno-variadic-macros -Werror=return-type)

# Using imported target from pkg-config; no manual include/link directories needed
target_compile_definitions(particle_sim PRIVATE GL_SILENCE_DEPRECATION)

if(APPLE)
    target_link_libraries(particle_sim PkgConfig::GLFW "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
else()
    target_link_libraries(particle_sim PkgConfig::GLFW GL)
endif()
