CXX = g++
CXXFLAGS = -Iinclude -std=c++11 -Wall
LDFLAGS = 

SRC_DIR = src
INCLUDE_DIR = include
MAIN_DIR = main

# Common sources
COMMON_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
COMMON_OBJECTS = $(COMMON_SOURCES:$(SRC_DIR)/%.cpp=$(SRC_DIR)/%.o)
COMMON_OBJECTS := $(filter-out $(SRC_DIR)/SolutionB.o $(SRC_DIR)/SolutionC.o $(SRC_DIR)/SolutionR.o, $(COMMON_OBJECTS))

# Solutions
SOLUTIONS = SolutionB SolutionC SolutionR

# Default solution to build
DEFAULT_SOLUTION = Solution

all: $(SOLUTIONS)

$(SOLUTIONS): %: $(COMMON_OBJECTS) $(MAIN_DIR)/ttrp.o $(SRC_DIR)/%.o
	$(CXX) $(COMMON_OBJECTS) $(MAIN_DIR)/ttrp.o $(SRC_DIR)/$@.o -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MAIN_DIR)/ttrp.o: $(MAIN_DIR)/ttrp.cpp
	$(CXX) $(CXXFLAGS) -c $(MAIN_DIR)/ttrp.cpp -o $(MAIN_DIR)/ttrp.o

clean:
	rm -f $(COMMON_OBJECTS) $(MAIN_DIR)/ttrp.o $(SOLUTIONS) $(SRC_DIR)/*.o

.PHONY: clean
