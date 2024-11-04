CC=clang
SWIFTC=swiftc
DEBUGFLAGS=-g
CFLAGS=-Wall -fPIC $(DEBUGFLAGS)

INCLUDE_DIR=include
SRC_DIR=src
BUILD_DIR=build
TESTS_DIR=tests
TARGET=ortiwindow

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    SO_EXT = so
    LDFLAGS = -shared -Wl,-soname,lib$(TARGET).$(SO_EXT) -lwayland-client
    PATH_ENV_VAR=LD_LIBRARY_PATH=$(BUILD_DIR)
		SRC_SPECIFIC_DIR=$(SRC_DIR)/linux
		C_FILES=$(wildcard $(SRC_SPECIFIC_DIR)/*.c) $(SRC_DIR)/lib$(TARGET).c
else ifeq ($(UNAME_S),Darwin)
    SO_EXT = dylib
    LDFLAGS = -shared -Wl,-install_name,@rpath/lib$(TARGET).$(SO_EXT) -framework Cocoa
    PATH_ENV_VAR=DYLD_LIBRARY_PATH=$(BUILD_DIR)
endif

OBJS=$(patsubst $(SRC_SPECIFIC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_FILES))
TEST_FILES=$(wildcard $(TESTS_DIR)/*.c)
TEST_OBJS=$(patsubst $(TESTS_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_FILES))
TEST_EXEC=$(BUILD_DIR)/test_$(TARGET)

all: $(BUILD_DIR)/lib$(TARGET).$(SO_EXT)

# Create build directory
$(BUILD_DIR):
	mkdir -p $@
# Compile .c files to .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
# Compile /OS/.c files to .o files
$(BUILD_DIR)/%.o: $(SRC_SPECIFIC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
# Build shared library
$(BUILD_DIR)/lib$(TARGET).$(SO_EXT): $(OBJS) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) -o $@ $^

# -- TESTS ---------------------------------------------------------------------
# Compile /tests/.c files to .o files
$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Create test executable
$(TEST_EXEC): $(TEST_OBJS) $(BUILD_DIR)/lib$(TARGET).$(SO_EXT) | $(BUILD_DIR)
	$(CC) -o $@ $^ -L$(BUILD_DIR) -l$(TARGET)

# Run tests
test: $(BUILD_DIR)/lib$(TARGET).$(SO_EXT) $(TEST_EXEC)
	@echo "Running tests..."
	$(PATH_ENV_VAR) $(TEST_EXEC)
# -- END TEST ------------------------------------------------------------------

# Clean up compiled objects and build files
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: clean

