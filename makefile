CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -pthread
TARGET = MatrixMult.exe
SOURCES = $(wildcard *.cpp)

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)