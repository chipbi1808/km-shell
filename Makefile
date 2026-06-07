CC := gcc
CPPFLAGS := -Iinclude
CFLAGS := -Wall -Wextra -Wstrict-prototypes -std=c11 -D_DEFAULT_SOURCE

TARGET := bin/tiny_shell
BUILD_DIR := build

SRCS := \
	src/main.c \
	src/shell_loop.c \
	src/input_parser.c \
	src/execute.c \
	src/builtin.c

OBJS := $(SRCS:src/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS) | bin
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR) bin:
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)
