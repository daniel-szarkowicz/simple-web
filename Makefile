BUILD_DIR := ./build
SRC_DIR := ./src

CFLAGS := -Werror=all -Werror=extra -Wpedantic -fsanitize=address -ggdb -I.

TARGET := $(BUILD_DIR)/server
TEMPLATER := $(BUILD_DIR)/templ

SRCS := main.c http.c request.c util.c response.c posts.c log.c defer.c

OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

all: $(TARGET)

run: $(TARGET)
	$<

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# generate c-includable files from html using the templater
$(BUILD_DIR)/%.htmlc: $(TEMPLATER) $(SRC_DIR)/%.html | $(BUILD_DIR)
	$^ $@

# compile single source-file executables (used for the templater)
$(BUILD_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

# generate dependency files (used to generate generated code before compiling)
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -MG -Ibuild -MM -MF $@ -MT $(@:%.d=%.o) $<

$(BUILD_DIR):
	mkdir -p $@

-include $(DEPS)
