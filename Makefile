CXX = g++
CC  = gcc

CXXFLAGS = -Wall -Wextra -g -fPIC -std=c++17 -Isrc -Iinclude -Iinclude/imgui
CFLAGS   = -Wall -Wextra -g -fPIC -Isrc -Iinclude
LDFLAGS  = -shared

TARGET = supersupertux.so

CPP_SRC = $(shell find src -name '*.cpp')
C_SRC   = $(shell find src -name '*.c')
SRC     = $(CPP_SRC) $(C_SRC)

CPP_OBJ = $(CPP_SRC:src/%.cpp=build/%.o)
C_OBJ   = $(C_SRC:src/%.c=build/%.o)
OBJ     = $(CPP_OBJ) $(C_OBJ)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	LD_PRELOAD=$(PWD)/$(TARGET) supertux2

clean:
	rm -rf build $(TARGET)

.PHONY: all run clean
