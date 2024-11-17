DEPFLAGS := -MMD -MP
CFLAGS := -Wall -Wextra -fsanitize=address

BUILD_DIR := ./build
TARGET := $(BUILD_DIR)/server

SRCS := main.c http.c request.c util.c response.c

OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

run: $(TARGET)
	$<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.htmlc: %.html templ
	./templ $< $@

$(BUILD_DIR):
	mkdir -p $@

-include $(DEPS)
