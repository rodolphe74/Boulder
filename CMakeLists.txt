cmake_minimum_required(VERSION 3.10)

project(Boulder)

file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR})

find_package(raylib CONFIG REQUIRED)
add_executable(boulder main.cpp CaveDecoder.cpp Globals.cpp MapUtils.cpp)
target_link_libraries(boulder PRIVATE raylib)

# find_package(Allegro CONFIG REQUIRED)
# add_executable(boulder main.cpp Globals.cpp MapUtils.cpp CaveDecoder.cpp)
# target_link_libraries(boulder PRIVATE Allegro::allegro Allegro::allegro_ttf Allegro::allegro_font Allegro::allegro_main Allegro::allegro_image)