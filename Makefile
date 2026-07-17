# Variables
CXX = g++
CXXFLAGS = -Wall -Wextra -g
TARGET = build/compiler

# Explicit object files
SRCS = src/main.cpp src/lexer/lexer.cpp src/token/token.cpp
OBJS = src/main.o src/lexer/lexer.o src/token/token.o

# Final link step
$(TARGET): $(OBJS)
	mkdir -p build
	$(CXX) -o $(TARGET) $(OBJS)

# Pattern rule for compiling C++ files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf src/*.o src/**/*.o $(TARGET) src/build/

.PHONY: clean
