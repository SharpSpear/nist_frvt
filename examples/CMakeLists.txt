add_executable(faces_example example.cpp)

target_include_directories(faces_example PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(faces_example faces)
