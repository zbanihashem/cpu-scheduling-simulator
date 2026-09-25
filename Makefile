CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

TARGET = scheduler
TEST_TARGET = scheduler_tests
MULTICORE_TEST_TARGET = multicore_tests
MULTICORE_SRTF_TEST_TARGET = multicore_srtf_tests
MULTICORE_RR_TEST_TARGET = multicore_rr_tests
MLFQ_TEST_TARGET = mlfq_tests

SRC = src/main.cpp \
      src/scheduler.cpp \
      src/multicore_scheduler.cpp

OBJ = $(SRC:.cpp=.o)

TEST_SRC = tests/test_scheduler.cpp \
           src/scheduler.cpp

MULTICORE_TEST_SRC = tests/test_multicore_scheduler.cpp \
                     src/multicore_scheduler.cpp

MULTICORE_SRTF_TEST_SRC = tests/test_multicore_srtf.cpp \
                          src/multicore_scheduler.cpp

MULTICORE_RR_TEST_SRC = tests/test_multicore_rr.cpp \
                        src/multicore_scheduler.cpp

MLFQ_TEST_SRC = tests/test_mlfq.cpp \
                src/scheduler.cpp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET)

$(MULTICORE_TEST_TARGET): $(MULTICORE_TEST_SRC)
	$(CXX) $(CXXFLAGS) $(MULTICORE_TEST_SRC) -o $(MULTICORE_TEST_TARGET)

$(MULTICORE_SRTF_TEST_TARGET): $(MULTICORE_SRTF_TEST_SRC)
	$(CXX) $(CXXFLAGS) $(MULTICORE_SRTF_TEST_SRC) -o $(MULTICORE_SRTF_TEST_TARGET)

$(MULTICORE_RR_TEST_TARGET): $(MULTICORE_RR_TEST_SRC)
	$(CXX) $(CXXFLAGS) $(MULTICORE_RR_TEST_SRC) -o $(MULTICORE_RR_TEST_TARGET)

$(MLFQ_TEST_TARGET): $(MLFQ_TEST_SRC)
	$(CXX) $(CXXFLAGS) $(MLFQ_TEST_SRC) -o $(MLFQ_TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET) \
      $(MULTICORE_TEST_TARGET) \
      $(MULTICORE_SRTF_TEST_TARGET) \
      $(MULTICORE_RR_TEST_TARGET) \
      $(MLFQ_TEST_TARGET)
	./$(TEST_TARGET)
	./$(MULTICORE_TEST_TARGET)
	./$(MULTICORE_SRTF_TEST_TARGET)
	./$(MULTICORE_RR_TEST_TARGET)
	./$(MLFQ_TEST_TARGET)

clean:
	rm -f $(OBJ) \
	      $(TARGET) \
	      $(TEST_TARGET) \
	      $(MULTICORE_TEST_TARGET) \
	      $(MULTICORE_SRTF_TEST_TARGET) \
	      $(MULTICORE_RR_TEST_TARGET) \
	      $(MLFQ_TEST_TARGET)

rebuild: clean all

.PHONY: all run test clean rebuild