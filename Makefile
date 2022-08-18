SOURCES = src/main.cpp 
SOURCES+= src/util.cpp
SOURCES+= src/util.hpp
SOURCES+= src/vertex.hpp
SOURCES+= src/mesh.cpp
SOURCES+= src/mesh.hpp
SOURCES+= vendor/src/glad.c
SOURCES+= vendor/src/stbimage.cpp

CFLAGS = `pkg-config glfw3 glm --cflags` -I vendor/include/
LIBS = `pkg-config glfw3 glm --libs`

CXX = g++


default: $(SOURCES)
	$(CXX) $(SOURCES) $(CFLAGS) $(LIBS) -o game
	
