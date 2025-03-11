# *****************************************************
# Variables to control Makefile operation
 
CC = g++-11
CFLAGS = -pthread -O3 --std=c++2a -g
TARGET = containers

# For deleting the target
TARGET_DEL = containers
 
# ****************************************************
# Targets needed to bring the executable up to date

# Source files
SRCS = main.cpp containers.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default rule to build and run the executable
all: $(TARGET)

# Rule to link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
#$(CC) $< $(CFLAGS) -o $@

# Clean rule to remove generated files
clean:
	$(RM) $(TARGET_DEL) $(OBJS) *.ans *.sol