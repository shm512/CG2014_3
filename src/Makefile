XX = g++
CXXFLAGS = -O2 -g -Wall -std=c++0x

# Strict compiler options
#CXXFLAGS += -Werror -Wformat-security -Wignored-qualifiers -Winit-self \
#		-Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith \
#		-Wtype-limits -Wempty-body -Wlogical-op \
#		-Wmissing-field-initializers -Wctor-dtor-privacy \
#		-Wnon-virtual-dtor -Wstrict-null-sentinel \
#		-Woverloaded-virtual -Wsign-promo -Weffc++

# Directories with source code
SRC_DIR = Triangles

INCLUDE_DIR += glm
CXXFLAGS += -I $(INCLUDE_DIR) 

BUILD_DIR = ../bin
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

LDFLAGS = -lGL -lglut -lGLEW

# All source files in our project (without libraries!)
CXXFILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJFILES := $(CXXFILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target (make without specified target).
.DEFAULT_GOAL := all

all: $(BIN_DIR)/main
	cp $(SRC_DIR)/*.vert	$(BIN_DIR)
	cp $(SRC_DIR)/*.frag	$(BIN_DIR)

# Suppress makefile rebuilding.
Makefile: ;


# Helper macros
make_path = $(addsuffix $(1), $(basename $(subst $(2), $(3), $(4))))
# Takes path to source file and returns path to corresponding object
src_to_obj = $(call make_path,.o, $(SRC_DIR), $(OBJ_DIR), $(1))


# Rules for compiling targets
# b
$(BIN_DIR)/main: $(OBJFILES)
#$(OBJ_DIR)/main.o $(OBJ_DIR)/shaderprogram.o $(OBJ_DIR)/MyObject.o $(OBJ_DIR)/shader.o $(OBJ_DIR)/Branch.o $(OBJ_DIR)/Leaf.o
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(filter %.o, $^) -o $@ $(LDFLAGS)


# Pattern for compiling object files (*.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(call src_to_obj, $<) $<

# Delete all temprorary and binary files
clean:
	rm -rf $(BUILD_DIR) 


