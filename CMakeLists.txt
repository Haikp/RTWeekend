cmake_minimum_required(VERSION 3.20)
project(RayTracingInOneWeekend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(app src/main.cc)

set(OSBitness 32)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OSBitness 64)
endif()

set (FullOutputDir "${CMAKE_SOURCE_DIR}/build/bin/${CMAKE_SYSTEM_NAME}${OSBitness}/${CMAKE_BUILD_TYPE}")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static libs")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}")

set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${FullOutputDir}
)

#### MODULES ####
add_subdirectory(modules/vec3)
add_subdirectory(modules/hittable)
add_subdirectory(modules/rtweekend)
add_subdirectory(modules/interval)
add_subdirectory(modules/camera)

target_link_libraries(app PRIVATE vec3)
target_link_libraries(app PRIVATE hittable)
target_link_libraries(app PRIVATE rtweekend)
target_link_libraries(app PRIVATE interval)
target_link_libraries(app PRIVATE camera)
