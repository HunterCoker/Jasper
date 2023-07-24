cmake_minimum_required(VERSION 3.22.1)

project(JasperGUI)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3")

set(JASP_ENGINE_SOURCE_DIR "../Jasper-Engine/src")
add_library(jasp OBJECT
	"${JASP_ENGINE_SOURCE_DIR}/util.cpp"
	"${JASP_ENGINE_SOURCE_DIR}/context.cpp"
	"${JASP_ENGINE_SOURCE_DIR}/stream.cpp"
	"${JASP_ENGINE_SOURCE_DIR}/detection.cpp"
	"${JASP_ENGINE_SOURCE_DIR}/target.cpp"
	"${JASP_ENGINE_SOURCE_DIR}/jasp_internal.cpp")

	
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL" EXCLUDE_FROM_ALL)

set(IMGUI_SOURCES
	"./vendor/imgui/imgui.cpp"
	"./vendor/imgui/imgui_draw.cpp"
	"./vendor/imgui/imgui_tables.cpp"
	"./vendor/imgui/imgui_widgets.cpp"
	"./vendor/imgui/backends/imgui_impl_sdl3.cpp"
	"./vendor/imgui/backends/imgui_impl_sdlrenderer3.cpp")
set(SOURCES main.cpp application.cpp ${IMGUI_SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES} $<TARGET_OBJECTS:jasp>)

target_include_directories(${PROJECT_NAME}
	PRIVATE "/usr/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Jasper-Engine/include"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include")

target_link_libraries(JasperGUI
	PRIVATE SDL3::SDL3
	PRIVATE realsense2)