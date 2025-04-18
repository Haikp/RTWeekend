add_library(hittable INTERFACE)
add_library(sphere INTERFACE)
add_library(hittable_list INTERFACE)

target_include_directories(hittable INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
