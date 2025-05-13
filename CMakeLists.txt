cmake_minimum_required(VERSION 3.20.0)
project(pt_gen VERSION 1.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the 'include' directory for headers
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(pt_gen
    src/main.cpp
)
find_package(TBB REQUIRED)

target_link_libraries(pt_gen PRIVATE TBB::tbb)
