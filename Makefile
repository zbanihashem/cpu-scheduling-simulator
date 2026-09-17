CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

TARGET = scheduler
TEST_TARGET = scheduler_tests

SRC = src/main.cpp \
      src/scheduler.cpp

OBJ = $(SRC:.cpp=.o)

TEST_SRC = tests/test_scheduler.cpp \
           src/scheduler.cpp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)

rebuild: clean all

.PHONY: all run test clean rebuild