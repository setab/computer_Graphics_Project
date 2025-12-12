CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra
FRAMEWORKS = -framework OpenGL -framework GLUT -framework OpenAL
TARGET = pong_game
SOURCES = main.cpp renderer.cpp physics.cpp input.cpp audio.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(FRAMEWORKS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
