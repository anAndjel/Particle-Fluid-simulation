cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O0 -g -ggdb3 -DDEBUG -Wall -Wextra -Wshadow -Wpedantic -Wno-variadic-macros -Wno-multistatement-macros -Wstringop-overflow -Werror=return-type") 

project(particle_sim)

include_directories(fluidsim/)
include_directories(fluidsim/extern/imgui)
include_directories(fluidsim/extern/implot)

add_executable(particle_sim
    fluidsim/main.cpp

    fluidsim/extern/imgui/imgui.cpp
    fluidsim/extern/imgui/imgui_demo.cpp
    fluidsim/extern/imgui/imgui_draw.cpp
    fluidsim/extern/imgui/imgui_tables.cpp
    fluidsim/extern/imgui/imgui_widgets.cpp

    fluidsim/extern/implot/implot.cpp
    fluidsim/extern/implot/implot_demo.cpp
    fluidsim/extern/implot/implot_items.cpp
)

target_link_libraries(particle_sim
    SFML
)
