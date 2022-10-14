# Generic stuff, ignore please
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
_mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
I := $(patsubst %/,%,$(dir $(_mkfile_path)))

ifneq ($(words $(MAKECMDGOALS)),1)
.DEFAULT_GOAL = all
%:
	@$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST))
else
ifndef ECHO
ifeq ($(OS), Windows_NT)
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory -nrRf $(firstword $(MAKEFILE_LIST)) ECHO="COUNTTHIS" | find /c "COUNTTHIS")
else
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory -nrRf $(firstword $(MAKEFILE_LIST)) ECHO="COUNTTHIS" | grep -c "COUNTTHIS")
endif
N := x
C = $(words $N)$(eval N := x $N)
ECHO = echo [$C/$T]
endif

BUILD_DIR ?= bin
SOURCE_DIR := src
TARGET_EXEC := comp.exe

CC := gcc

CONFIG ?= Release

# Release config
ifeq ($(CONFIG),Release)
CFLAGS ?= -m64 -std=c17 -flto -Ofast -ffunction-sections -fdata-sections
LDFLAGS ?= -m64 -flto -Ofast -s -Wl,--gc-sections,--as-needed
else ifeq ($(CONFIG),Debug) # Debug
CFLAGS ?= -m64 -std=c17 -Og -ggdb
LDFLAGS ?= -m64 -Og -ggdb
endif

BUILD_DIRS ?= $(sort $(dir $(OBJ)))
SRC := $(call rwildcard,$(SOURCE_DIR),*.c)
OBJ := $(SRC:%=$(BUILD_DIR)/%.obj)
-include $(OBJ:%=$(BUILD_DIR)/%.dep)

all: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(BUILD_DIR) $(OBJ)
	@$(ECHO) Linking $<
	@$(CC) $(OBJ) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.c.obj: %.c
	@$(ECHO) Building $<
	@$(CC) $(CFLAGS) -MD -MF $@.dep -c $< -o $@

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

endif # This is not a mistake, its intentional