CXX = g++
CXXFLAGS = -Wall -Wextra --std=c++17 -g -O3
LIB_FLAGS = -lrealsense2 -lSDL3 -lGL

DIRECTORIES = ${include_dirs} ${library_dirs}
include_dirs = -I/usr/include -I./vendor/SDL/include -I$(IMGUI_DIR) -I$(IMGUI_DIR)backends -I../Jasp-Engine
library_dirs = -L./vendor/SDL/build
BUILD_DIR = ./build/

TARGET := objdetectionv1

IMGUI_DIR = ./vendor/imgui/
IMGUI_SRC = $(IMGUI_DIR)imgui.cpp $(IMGUI_DIR)imgui_draw.cpp $(IMGUI_DIR)imgui_tables.cpp $(IMGUI_DIR)imgui_widgets.cpp $(IMGUI_DIR)/backends/imgui_impl_sdl3.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer3.cpp

JASP_DIR = ../Jasp-Engine/
JASP_SRC = $(JASP_DIR)util.cpp $(JASP_DIR)context.cpp $(JASP_DIR)stream.cpp $(JASP_DIR)target.cpp $(JASP_DIR)jasp_internal.cpp 

SRCS := main.cpp application.cpp
SRCS += $(IMGUI_SRC)
SRCS += $(JASP_SRC)

all: $(TARGET)

${TARGET}:
	$(CXX) $(CXXFLAGS) $(DIRECTORIES) $(SRCS) -o $(TARGET) $(LIB_FLAGS)
	mkdir -p ${BUILD_DIR}
	mv -f $(TARGET) ${BUILD_DIR}

clean:
	rm -r ${BUILD_DIR}