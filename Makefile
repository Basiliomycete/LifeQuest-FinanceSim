CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = simlife
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC) SimLife.h
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) savegame.txt

run: all
	./$(TARGET)
