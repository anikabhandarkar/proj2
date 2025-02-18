CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -I/usr/include/gtest -I/usr/include/gtest
LDFLAGS = -L/usr/lib -lgtest -lgtest_main -pthread -lexpat

SRC_DIR = src
TEST_SRC_DIR = testsrc
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJS = $(TEST_SRCS:$(TEST_SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/teststrutils

all: directories $(TARGET)

directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(TEST_OBJS) | directories
	$(CXX) $(OBJS) $(TEST_OBJS) $(LDFLAGS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
