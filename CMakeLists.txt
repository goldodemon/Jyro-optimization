cmake_minimum_required(VERSION 3.16)
project(JyroOptimization VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(jyro-optimization src/main.cpp)

if (WIN32)
    target_compile_definitions(jyro-optimization PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(jyro-optimization PRIVATE shell32 advapi32)
endif()
