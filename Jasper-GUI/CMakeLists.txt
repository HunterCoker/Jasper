cmake_minimum_required(VERSION 3.22.1)

project(JasperGUI)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Jasper-Engine" "Jasper-Engine")

set(BUILD_SHARED_LIBS OFF)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL" EXCLUDE_FROM_ALL)

set(IMGUI_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_sdl3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_sdlrenderer3.cpp")
set(SOURCES main.cpp application.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME}
	PRIVATE "/usr/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Jasper-Engine/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb")

target_link_libraries(${PROJECT_NAME}
	PRIVATE SDL3::SDL3
	PRIVATE jasp)