CC=clang
DEBUGFLAGS=-g 
CFLAGS=-Wall -fPIC $(DEBUGFLAGS)
INCLUDE_DIR=include
SRC_DIR=src
BUILD_DIR=build
TESTS_DIR=tests
TARGET=ortiarena

# Source
SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) 

# Tests
TEST_SRCS = $(wildcard $(TESTS_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TESTS_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_EXEC = $(BUILD_DIR)/test_$(TARGET)

all: $(BUILD_DIR)/lib$(TARGET).so

# Rule create build dir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to compile .c files into .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to build the shared library
$(BUILD_DIR)/lib$(TARGET).so: $(OBJS)
	$(CC) -shared -o $@ $^

# -- TEST -------------
# Create test executable
$(TEST_EXEC): $(TEST_OBJS) $(BUILD_DIR)/lib$(TARGET).so
	$(CC) -o $@ $^ -L$(BUILD_DIR) -l$(TARGET)

# Rule to compile test .c files into .o files
$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Tests: compile and link test executable
test: $(BUILD_DIR)/lib$(TARGET).so $(TEST_EXEC)
	@echo "Running tests..."
	LD_LIBRARY_PATH=$(BUILD_DIR) $(TEST_EXEC)

test-valgrind: $(BUILD_DIR)/lib$(TARGET).so $(TEST_EXEC)
	@echo "Running valgrind..."
	LD_LIBRARY_PATH=$(BUILD_DIR) valgrind --leak-check=full $(TEST_EXEC)

# -- END TEST -------------

# Clean up compiled objects and build files
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/lib$(TARGET).so $(TEST_EXEC)

# Phony targets
.PHONY: all clean

