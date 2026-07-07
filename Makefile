CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Iinclude
LDFLAGS = -L. -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

TARGET = game.exe

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
