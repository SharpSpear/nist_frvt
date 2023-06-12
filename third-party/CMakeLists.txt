add_subdirectory(miniconf)

add_library(semimap INTERFACE)
target_include_directories(semimap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/semimap)