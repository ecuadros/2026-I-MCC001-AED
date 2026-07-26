CXX = g++
CXXFLAGS = -std=c+2b
TARGET = main
#SRC = demoMap.cpp
SRC = digitalTreeDemo.cpp
#SRC = demoLinkedListSTL.cpp

all: $(TARGET)
$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)