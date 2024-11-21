CC := gcc

CFLAGS := 
CFLAGS_DEBUG := $(CFLAGS) -g -DDEBUG -fsanitize=address -Wall -Wextra -pedantic
CFLAGS_RELEASE := $(CFLAGS) -O2 -DNDEBUG

LDFLAGS := -L/opt/homebrew/lib -llua
LDFLAGS_DEBUG := $(LDFLAGS) -fsanitize=address
LDFLAGS_RELEASE := $(LDFLAGS)

PROJECT_DIR := .
INC=-I./include -I/opt/homebrew/include
BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

SRCS := $(shell find $(PROJECT_DIR) -name "*.c")
OBJS := $(SRCS:$(PROJECT_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET := $(BIN_DIR)/lsr

.PHONY: all debug release clean format test install uninstall completions

all: debug

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LDFLAGS += $(LDFLAGS_DEBUG)
debug: $(TARGET)

release: CFLAGS += $(CFLAGS_RELEASE)
release: LDFLAGS += $(LDFLAGS_RELEASE)
release: clean
release: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $(INC) $^ -o $@

$(OBJ_DIR)/%.o: $(PROJECT_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	-mkdir -p $@
	-mkdir -p $(shell dirname $(OBJS))

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	rm -rf completions

format:
	clang-format -i $(SRCS) $(PROJECT_DIR)/include/**.h

completions:
	@mkdir -p completions
	@mkdir -p completions/bash
	@mkdir -p completions/zsh
	@mkdir -p completions/fish

	./$(TARGET) --completions bash > completions/bash/lsr
	./$(TARGET) --completions zsh > completions/zsh/_lsr
	./$(TARGET) --completions fish > completions/fish/lsr.fish

install: release completions
	install -m 755 $(TARGET) /usr/local/bin
	make clean

uninstall:
	rm -f /usr/local/bin/$(notdir $(TARGET))

