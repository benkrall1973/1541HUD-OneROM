TOOLCHAIN ?=
.DEFAULT_GOAL := all
CC := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-gcc,arm-none-eabi-gcc)
OBJCOPY := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-objcopy,arm-none-eabi-objcopy)
MAJOR_VERSION := 0
MINOR_VERSION := 8
PATCH_VERSION := 1
BUILD_VERSION := 31
# Keep this relative for the native Windows GNU Make path; an absolute C:/
# prefix is parsed as Make syntax in generated object prerequisites.
ONEROM_ROOT ?= ../../../.work/official-v0.8.1/one-rom-v0.8.1
ORA_DIR := $(ONEROM_ROOT)/firmware/ora
BUILD_DIR := build
PLUGIN_PREFIX := 1541-OneROM-1541HUD-Monitor-v$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION).$(BUILD_VERSION)
ELF := $(BUILD_DIR)/$(PLUGIN_PREFIX).elf
BIN := $(BUILD_DIR)/$(PLUGIN_PREFIX).bin
SRC := src/hud1541_monitor_main.c src/flash_erase.c
CFLAGS := -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O3 -Wall -Wextra -Werror -ffunction-sections -fdata-sections -fstack-usage -fconserve-stack -I$(ORA_DIR) -Isrc -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -DPATCH_VERSION=$(PATCH_VERSION) -DBUILD_VERSION=$(BUILD_VERSION)
RWX_SEGMENT_FLAG ?= -Wl,--no-warn-rwx-segments
LDFLAGS := -nostdlib -L $(ORA_DIR) -T hud1541_monitor_plugin.ld -Wl,--fatal-warnings $(RWX_SEGMENT_FLAG) -Wl,-Map=$(BUILD_DIR)/$(PLUGIN_PREFIX).map
OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
$(BUILD_DIR)/src/flash_erase.o: CFLAGS += -fPIC -fno-plt
all: $(BIN)
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
$(ELF): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -lc -lgcc -o $@
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(OBJCOPY) --pad-to 0x10030000 --gap-fill 0xFF -O binary $< $@
$(BUILD_DIR):
	@mkdir -p $@
clean:
	@rm -rf $(BUILD_DIR)
