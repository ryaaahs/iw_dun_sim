CC := gcc
CFLAGS := -Wall -Wextra -std=c89 -pedantic

SRC_DIR := src
BUILD_DIR := build

# Pattern match all the C files within source
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Converts the filepath to the target directory we want
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TARGET := $(BUILD_DIR)/iw_dun_sim

all: $(BUILD_DIR) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# $^ All prerequisites of the rule (places in $(OBJS))
# $@ Name of the target to produc (build/iw_dun_sim)
# Compile all the objects into the target (build/iw_dun_sim)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile each source c file into an object file into build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
