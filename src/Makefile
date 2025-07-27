# Compiler and flags
CXX = g++
CC  = gcc
CXXFLAGS = -std=c++23 -Wall 
CFLAGS   = -std=c11 -Wall

# Sources

Y_FILE   := parser.y
Y_SRC    := parserbase.cc
Y_HDR    := parserbase.h
C_SRCS   := 
CC_SRCS  := \
	$(Y_SRC) \
	main.cc \
	parser.cc \
	util.cc \
	direction.cc \
	dictionary.cc \
	action.cc \
	game.cc \
	item.cc \
	location.cc \
	player.cc \
	zobject.cc \
	condition.cc

# Object files
OBJS = $(C_SRCS:.c=.o) $(CC_SRCS:.cc=.o)

# Output binary
TARGET = zorked

all: $(TARGET)

# Build target
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^

# Rule to build .o from .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build .o from .cpp
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate parser.cpp and parser.hpp from parser.y
$(Y_SRC) $(Y_HDR): $(Y_FILE)
	bison --defines=$(Y_HDR) -o $(Y_SRC) $(Y_FILE)

# Bison generated files need to be built first
$(OBJS): $(Y_SRC) $(Y_HDR)

# Clean up
clean:
	rm -f *.o $(TARGET) $(Y_SRC) $(Y_HDR)

.PHONY: all clean
