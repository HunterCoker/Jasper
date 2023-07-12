CXX = g++
CXXFLAGS = -Wall -O0 --std=c++14 -g
LIB_FLAGS = -lrealsense2 -lSDL2

DIRECTORIES = ${include_dirs} ${library_dirs}
include_dirs = -I/usr/include -I/usr/include/SDL2 -I$(IMGUI_DIR) -I$(IMGUI_DIR)backends
library_dirs =
BUILD_DIR = ./build/

TARGET := objdetectionv1

IMGUI_DIR = ../../dependencies/imgui-docking/
IMGUI_SRC = $(IMGUI_DIR)imgui.cpp $(IMGUI_DIR)imgui_demo.cpp $(IMGUI_DIR)imgui_draw.cpp $(IMGUI_DIR)imgui_tables.cpp $(IMGUI_DIR)imgui_widgets.cpp $(IMGUI_DIR)backends/imgui_impl_sdl2.cpp $(IMGUI_DIR)backends/imgui_impl_sdlrenderer2.cpp

SRCS := main.cpp
SRCS += $(IMGUI_SRC)

all: $(TARGET)

${TARGET}:
	$(CXX) $(CXXFLAGS) $(DIRECTORIES) $(SRCS) -o $(TARGET) $(LIB_FLAGS)
	mkdir -p ${BUILD_DIR}
	mv -f $(TARGET) ${BUILD_DIR}

clean:
	rm -r ${BUILD_DIR}