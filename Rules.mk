BUILD_DIR ?= bin
SOURCE_DIR := src
TARGET_EXEC := comp.exe

CC := gcc

CONFIG ?= Release

CFLAGS ?= -Iinclude

# Release config
ifeq ($(CONFIG),Release)
CFLAGS += -m64 -std=c17 -flto -Ofast -ffunction-sections -fdata-sections
LDFLAGS += -m64 -flto -Ofast -s -Wl,--gc-sections,--as-needed
else ifeq ($(CONFIG),Debug) # Debug
CFLAGS += -m64 -std=c17 -Og -ggdb
LDFLAGS += -m64 -Og -ggdb
endif

BUILD_DIRS ?= $(sort $(dir $(OBJ)))
SRC := $(call rwildcard,$(SOURCE_DIR),*.c)
OBJ := $(SRC:%=$(BUILD_DIR)/%.obj)
-include $(OBJ:%=%.dep)

all: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(BUILD_DIR) $(OBJ)
	@$(ECHO) Linking $<
	@$(CC) $(OBJ) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.c.obj: %.c
	@$(ECHO) Building $<
	@$(CC) $(CFLAGS) -MD -MP -MF $@.dep -c $< -o $@

clean:
ifeq ($(OS), Windows_NT)
	@rmdir /S /Q $(BUILD_DIR)
else
	@rm -rf $(BUILD_DIR)
endif

$(BUILD_DIR):
ifeq ($(OS), Windows_NT)
	@mkdir $(subst /,\,$(BUILD_DIRS))
else
	@mkdir -p $(BUILD_DIRS)
endif