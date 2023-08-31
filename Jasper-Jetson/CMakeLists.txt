cmake_minimum_required(VERSION 3.22.1)

project(JasperGUI)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Jasper-Engine" "Jasper-Engine")

add_executable(${PROJECT_NAME} main.cpp ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Jasper-Engine/include")

target_link_libraries(JasperGUI
        PRIVATE Jasp)