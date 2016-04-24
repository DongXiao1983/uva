CC  = gcc
CXX = g++

CPPUTEST_HOME = ./cpputest



TARGET = codeclean_utest

SRC = .


SRC_DIRS = ..

TEST_SRC_DIRS = \
	./


OBJ = ../xstring.o
OBJ += ../memory.o
OBJ += ../entry.o

TEST_FILE:=$(wildcard $(SRC)/*.cpp)
OBJ += $(TEST_FILE:.cpp=.o)


CPPFLAGS = -g -Wall  -I$(CPPUTEST_HOME)/include -I$(SRC_DIRS)
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

all: $(TARGET)
	./$(TARGET)

$(TARGET):$(OBJ)
	$(CXX)  -o $(TARGET)  $(OBJ) $(LD_LIBRARIES) 

%.o: %.c %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f ./*.o
	rm -f $(TARGET)
