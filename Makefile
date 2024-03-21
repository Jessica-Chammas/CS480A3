# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++11

# The build target executable
TARGET = pagingwithpr

# Source files
SRCS = Levels.cpp log_helpers.cpp main.cpp pagetable.cpp vaddr_tracereader.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(TARGET) $(OBJS)
