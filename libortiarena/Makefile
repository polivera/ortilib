CC=clang
DEBUGFLAGS=-g 
CFLAGS=-Wall -fPIC $(DEBUGFLAGS)
INCLUDE_DIR=include
SRC_DIR=src
BUILD_DIR=build
TESTS_DIR=tests
TARGET=ortiarena

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    SO_EXT = so
    LDFLAGS = -shared -Wl,-soname,lib$(TARGET).$(SO_EXT)
		PATH_VAR_NAME=LD_LIBRARY_PATH=$(BUILD_DIR):$$LD_LIBRARY_PATH
    OS_SPECIFIC_FILES=$(filter %_linux.c, $(wildcard $(SRC_DIR)/*_linux.c))
else ifeq ($(UNAME_S),Darwin)
    SO_EXT = dylib
    LDFLAGS = -shared -Wl,-install_name,@rpath/lib$(TARGET).$(SO_EXT)
    PATH_VAR_NAME=DYLD_LIBRARY_PATH=$(BUILD_DIR)
    OS_SPECIFIC_FILES=$(filter %_macos.c, $(wildcard $(SRC_DIR)/*_macos.c))
endif

# Source files (shared and OS-specific)
SHARED_SOURCE_FILES=$(filter-out %_linux.c %_macos.c, $(wildcard $(SRC_DIR)/*.c))
ALL_SOURCE_FILES=$(SHARED_SOURCE_FILES) $(OS_SPECIFIC_FILES)

# Object files
OBJS=$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(ALL_SOURCE_FILES))

# Test files (specify manually if needed)
TEST_FILES=$(wildcard $(TESTS_DIR)/*.c)
TEST_OBJS=$(patsubst $(TESTS_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_FILES))
TEST_EXEC=$(BUILD_DIR)/test_$(TARGET)

all: $(BUILD_DIR)/lib$(TARGET).$(SO_EXT)

# Rule to create build directory
$(BUILD_DIR):
	mkdir -p $@

# Rule to compile source .c files into .o files in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to build the shared library
$(BUILD_DIR)/lib$(TARGET).$(SO_EXT): $(OBJS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# --- TESTS ---
# Rule to create the test executable
$(TEST_EXEC): $(TEST_OBJS) $(BUILD_DIR)/lib$(TARGET).$(SO_EXT) | $(BUILD_DIR)
	$(CC) -o $@ $^ -L$(BUILD_DIR) -l$(TARGET)

# Rule to compile test .c files into .o files in the build directory
$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Run tests
test: $(BUILD_DIR)/lib$(TARGET).$(SO_EXT) $(TEST_EXEC)
	@echo "Running tests..."
	$(PATH_VAR_NAME) $(TEST_EXEC)

# Run tests with Valgrind
test-valgrind: $(BUILD_DIR)/lib$(TARGET).$(SO_EXT) $(TEST_EXEC)
	@echo "Running valgrind..."
	LD_LIBRARY_PATH=$(BUILD_DIR) valgrind --leak-check=full $(TEST_EXEC)

# Clean up compiled objects and build files
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean test test-valgrind
