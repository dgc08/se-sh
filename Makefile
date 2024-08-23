TARGET = se-sh-libc
TEST_FLAGS =

INCLUDE_DIRS = src/shell
SRC_DIRS = src/shell

SERIAL_MONITOR = pio device monitor
CC = cc
LINTER =  clang-tidy
COMMON_CFLAGS = $(INCLUDE_FLAGS) -Wextra -Wno-ignored-qualifiers
COMMON_LDFLAGS =

DEBUG_FLAGS = -fsanitize=address -g
RELEASE_FLAGS = -O2

DEBUG_LDFLAGS = -fsanitize=address
RELEASE_LDFLAGS = -s
TMP_DIR = tmp

####

INCLUDE_FLAGS = $(addprefix -I, $(INCLUDE_DIRS))

ESP32_LOCK = $(TMP_DIR)/ESP32.lock
ESP32_FS_LOCK = $(TMP_DIR)/ESP32_FS.lock
DEBUG_LOCK = $(TMP_DIR)/debug_libc.lock
RELEASE_LOCK = $(TMP_DIR)/release_libc.lock
OBJ_DIR = $(TMP_DIR)/obj

COMMON_SOURCES = $(shell find $(SRC_DIRS) -name '*.c')
SRCS := $(COMMON_SOURCES) src/libc_main.c
ESP_SRCS := $(COMMON_SOURCES) $(wildcard src/esp32*.cpp) $(wildcard src/generic_arduino*.cpp)
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

####

default: release

$(TMP_DIR)/%:
	@mkdir -p $(dir $@)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(TMP_DIR)/%libc.lock: $(OBJ_DIR)
	make clean_obj
	touch $@

$(ESP32_LOCK): $(ESP_SRCS)
	platformio run --target upload
	touch $@
$(ESP32_FS_LOCK) : $(wildcard data/*)
	platformio run --target uploadfs
	touch $@

.PHONY: default debug release lint-libc lint-common run clean clean_obj up con ucon upfs

release: CFLAGS = $(COMMON_CFLAGS) $(RELEASE_FLAGS)
release: LDFLAGS = $(COMMON_LDFLAGS) $(RELEASE_LDFLAGS)
release: $(RELEASE_LOCK) $(TARGET)

debug: CFLAGS = $(COMMON_CFLAGS) $(DEBUG_FLAGS)
debug: LDFLAGS = $(COMMON_LDFLAGS) $(DEBUG_LDFLAGS)
debug: $(DEBUG_LOCK) $(TARGET)

lint-libc: $(SRCS)
	$(LINTER) $^ -- $(COMMON_CFLAGS)
lint-common: $(COMMON_SOURCES)
	$(LINTER) $^ -- $(COMMON_CFLAGS)

run: debug
	echo && ./$(TARGET) $(TEST_FLAGS)

up: upfs $(ESP32_LOCK)

con:
	$(SERIAL_MONITOR)
ucon: up con
upfs: $(ESP32_FS_LOCK)

clean: clean_obj
	rm -rf $(TARGET) $(TMP_DIR)
	pio run --target clean
clean_obj:
	rm -rf $(OBJ_DIR)
