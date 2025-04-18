add_library(vec3 INTERFACE)
add_library(color INTERFACE)
add_library(ray INTERFACE)

target_include_directories(vec3 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
