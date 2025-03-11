# *****************************************************
# Variables to control Makefile operation
 
CC = g++-11
CFLAGS = --std=c++2a -pthread -O3 -g
TARGET1 = counter
TARGET2 = mysort

# For deleting the target
TARGET_DEL = counter
 
# ****************************************************
# Targets needed to bring the executable up to date

# Source files
SRCS1 = counter.cpp locks_bar.cpp
SRCS2 = main.cpp Sort_p.cpp locks_bar.cpp

# Object files
OBJS1 = $(SRCS1:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)

# Default rule to build and run the executable
all:  $(TARGET1) $(TARGET2)

example: exampleC exampleCPP 

exampleCPP: exampleCPP.cpp
	g++ exampleCPP.cpp -pthread -O3 -std=c++2a -g -o exampleCPP

exampleC: exampleC.c
	gcc exampleC.c -pthread -O3 -g -o exampleC

.PHONY : clean
example_clean:
	rm exampleC exampleCPP

# Rule to link object files into the target executable
$(TARGET1): $(OBJS1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS1)
	
# Rule to link object files into the target executable
$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJS2)	

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	$(RM) $(TARGET_DEL) $(TARGET2) *.ans *.sol