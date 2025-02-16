BIN_DIR = bin
OBJ_DIR = obj  

all: directories runtests

build: g++ CDSV.cpp StringDataSink.cpp StringDataSource.cpp
	g++ -std=c++11 -o CDSV.cpp StringDataSink.cpp StringDataSource.cpp

runtests: $(TARGET)
	$(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(OBJ_FILES) $(LDFLAGS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

$(OBJ_DIR)/%.o: $(TESTSRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

clean::
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
