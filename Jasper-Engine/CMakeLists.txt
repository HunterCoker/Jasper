cmake_minimum_required(VERSION 3.22.1)
project(JasperEngine)

add_library(jasp STATIC
    "src/Jasp.cpp"
    "src/Util.cpp"
    "src/Context.cpp"
    "src/computer_vision/Stream.cpp"
    "src/computer_vision/Detection.cpp"
    "src/computer_vision/Target.cpp"
    "src/JaspInternal.cpp")

target_include_directories(jasp
    PRIVATE "include")

target_link_libraries(jasp
    PRIVATE realsense2)
