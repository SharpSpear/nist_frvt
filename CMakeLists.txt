cmake_minimum_required(VERSION 3.13)
project(faces)

set(CMAKE_CXX_STANDARD 17)

find_package(spdlog REQUIRED)
find_package(OpenCV 4 REQUIRED)
find_package(dlib REQUIRED)

add_compile_definitions(FACES_ROOT_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")

add_library(faces "")

target_link_libraries(faces ${OpenCV_LIBS})
target_link_libraries(faces dlib::dlib)
target_link_libraries(faces spdlog::spdlog)

include_directories(src)

add_subdirectory(third-party)
target_link_libraries(faces miniconf)
target_link_libraries(faces semimap)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(doc)
