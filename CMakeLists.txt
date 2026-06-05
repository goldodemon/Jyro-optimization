cmake_minimum_required(VERSION 3.20)
project(JyroOptimization VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(jyro_core
    src/OptimizationCatalog.cpp
)
target_include_directories(jyro_core PUBLIC include)

if(WIN32)
    add_executable(JyroOptimization WIN32 src/main.cpp)
    target_link_libraries(JyroOptimization PRIVATE jyro_core winhttp shell32 advapi32 gdi32 user32 dwmapi)
    target_compile_definitions(JyroOptimization PRIVATE UNICODE _UNICODE)
else()
    add_executable(JyroOptimization src/main.cpp)
    target_link_libraries(JyroOptimization PRIVATE jyro_core)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(jyro_catalog_test tests/catalog_test.cpp)
    target_link_libraries(jyro_catalog_test PRIVATE jyro_core)
    add_test(NAME jyro_catalog_test COMMAND jyro_catalog_test)
endif()
