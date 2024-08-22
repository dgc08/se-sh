# Default target
all: release

con: # Serial monitor generic arduino
	pio device monitor

TARGET = se-sh-libc
TEST_FLAGS =

SRC_DIRS = src/shell
INCLUDE_DIRS = src/shell

# Compiler and flags
CC = cc
INCLUDE_FLAGS = $(addprefix -I, $(INCLUDE_DIRS))
COMMON_CFLAGS = $(INCLUDE_FLAGS) -Wall -Wextra -Wno-discarded-qualifiers -Wno-ignored-qualifiers

LINTER =  clang-tidy

CFLAGS_DEBUG = -g -O0 -Wall
CFLAGS_RELEASE = -O3 -Wall

DEBUG_FLAGS = -fsanitize=address -g
RELEASE_FLAGS = -O3

COMMON_LDFLAGS =
DEBUG_LDFLAGS = -fsanitize=address
RELEASE_LDFLAGS = -s

# The directory for object files
ESP32_LOCK = $(TMP_DIR)/ESP32.lock
DEBUG_LOCK = $(TMP_DIR)/debug.lock
RELEASE_LOCK = $(TMP_DIR)/release.lock
TMP_DIR = tmp
OBJ_DIR = $(TMP_DIR)/obj

# Find all .c files in SRC_DIRS
SHELL_SRCS = $(shell find $(SRC_DIRS) -name '*.c')
SRCS := $(SHELL_SRCS) src/libc_main.c
ESP_SRCS := $(SHELL_SRCS) $(wildcard src/esp32*.cpp) $(wildcard src/generic_arduino*.cpp)

# Generate the object file names by replacing .c with .o and prefixing with OBJ_DIR/
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all debug release lint-libc lint-esp lint-shell test clean clean_obj up con ucon

debug: CFLAGS = $(COMMON_CFLAGS) $(DEBUG_FLAGS)
debug: LDFLAGS = $(COMMON_LDFLAGS) $(DEBUG_LDFLAGS)
debug: $(DEBUG_LOCK) $(TARGET)

release: CFLAGS = $(COMMON_CFLAGS) $(RELEASE_FLAGS)
release: LDFLAGS = $(COMMON_LDFLAGS) $(RELEASE_LDFLAGS)
release: $(RELEASE_LOCK) $(TARGET)

lint-esp: $(ESP_SRCS)
	pio check
lint-libc: $(SRCS)
	$(LINTER) $^ -- $(COMMON_CFLAGS)
lint-shell: $(SHELL_SRCS)
	$(LINTER) $^ -- $(COMMON_CFLAGS)


test: debug
	echo && ./$(TARGET) $(TEST_FLAGS)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(TMP_DIR)/%.lock: $(OBJ_DIR)
	make clean_obj
	touch $@

# Compile .c files into .o files for release
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)  # Ensure the directory for the object file exists
	$(CC) $(CFLAGS) -c -o $@ $<

$(ESP32_LOCK): $(ESP_SRCS)
	@mkdir -p $(dir $@)
	platformio run --target upload
	touch $@

up: $(ESP32_LOCK)
ucon: $(ESP32_LOCK) con
udev: $(ESP32_LOCK) dev

# Clean up the build
clean: clean_obj
	rm -rf $(TARGET)
	pio run --target clean
clean_obj:
	rm -rf $(OBJ_DIR)
