CC = g++
CFLAGS = -Wall -std=c++14
TARGET = enigma
SRCS = enigma.cpp
OBJS = $(SRCS:%.cpp=%.o)
DEPS = $(SRCS:%.cpp=%.d)

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

.PHONY: clean
