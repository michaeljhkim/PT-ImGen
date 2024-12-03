cmake_minimum_required(VERSION 3.20.0)
project(ray_trace_assignment VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the 'include' directory for headers
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(ray_trace_assignment
    src/main.cpp
)
find_package(TBB REQUIRED)

target_link_libraries(ray_trace_assignment PRIVATE TBB::tbb)
