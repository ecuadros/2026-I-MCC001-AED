CXX = g++
CXXFLAGS = -std=c++2b -Wall -g -pthread -I. -Icontainers # Añadido -pthread
LDFLAGS = -pthread # Añadido -pthread

TARGET = main
SRCS = main.cpp HeapDemo.cpp BinaryTreeDemo.cpp AVLTreeDemo.cpp containers/BTreeDemo.cpp


#ListsDemo.cpp DLLDemo.cpp 	   
# 	   DemoVector.cpp
# macros.cpp vector.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean