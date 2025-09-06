CC := gcc

CFLAGS := -Wall -Wextra -std=c99 -pedantic -g

SRC_DIR := src
BUILD_DIR := build
SIM_JSON := json/simulation_values.json
# Default values for arugments
MARKET_PARSE := 1
DISPLAY_TYPE := 100

# Pattern match all the C files within source
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Converts the filepath to the target directory we want
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TARGET := $(BUILD_DIR)/iw_dun_sim

all: $(TARGET)

# $^ All prerequisites of the rule (places in $(OBJS))
# $@ Name of the target to produc (build/iw_dun_sim)
# Compile all the objects into the target (build/iw_dun_sim)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile each source c file into an object file into build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR):
# Create build directory if it doesn't exist
ifeq ($(OS), Windows_NT)
	mkdir $(BUILD_DIR)
else
	mkdir -p $(BUILD_DIR)
endif

clean:
ifeq ($(OS), Windows_NT)
	del /s /q $(BUILD_DIR)
else
	rm -rf $(BUILD_DIR)
endif
	
run: 
	$(TARGET) $(SIM_JSON) $(MARKET_PARSE) $(DISPLAY_TYPE)

help:
	$(TARGET) $(SIM_JSON)
