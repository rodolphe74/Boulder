cmake_minimum_required(VERSION 3.10)

project(Boulder)
set (CMAKE_CXX_STANDARD 11)
file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # xcode workaround
    file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR}/Debug)
endif()

find_package(glfw3 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
add_executable(boulder main.cpp CaveDecoder.cpp Globals.cpp MapUtils.cpp)
target_link_libraries(boulder PRIVATE raylib glfw)


# Project creation on MacOs
# git clone https://github.com/microsoft/vcpkg.git
# cd vcpkg
# ./vcpkg install glfw3
# ./vcpkg install ray lib
# git clone https://github.com/rodolphe74/Boulder.git
# mkdir debug
# cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg_folder]/scripts/buildsystems/vcpkg.cmake -GXcode
