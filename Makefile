CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

TARGET = scheduler

SRC = src/main.cpp \
      src/scheduler.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all

.PHONY: all run clean rebuild