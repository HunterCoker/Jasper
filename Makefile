CXX = g++
CXXFLAGS = -Wall -O0 --std=c++14 -g
LIB_FLAGS = -lrealsense2 -lSDL2

DIRECTORIES = ${include_dirs} ${library_dirs}
include_dirs = -I/usr/include/ -I/usr/include/SDL2/
library_dirs =
build_dir = ./build/

TARGET := objdetectionv1
SRCS := main.cpp Application.cpp

all: $(TARGET)

${TARGET}:
	$(CXX) $(CXXFLAGS) $(DIRECTORIES) $(SRCS) -o $(TARGET) $(LIB_FLAGS)
	rm -r ./build/
	mkdir build
	mv $(TARGET) ${build_dir}

clean:
	rm $(TARGET)