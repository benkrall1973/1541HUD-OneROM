# Copyright (C) 2026 Piers Finlayson <piers@piers.rocks>
#
# MIT License

TOOLCHAIN ?=
CC      := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-gcc,arm-none-eabi-gcc)
OBJCOPY := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-objcopy,arm-none-eabi-objcopy)
OBJDUMP := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-objdump,arm-none-eabi-objdump)

MAJOR_VERSION := 0
MINOR_VERSION := 8
PATCH_VERSION := 1
BUILD_VERSION := 5
WORK_BUILD := 5


# GNU ld 2.39+ warns about the flat plugin's deliberate RWX load segment.
# The qualified GNU Arm 15.3.Rel1 linker understands the suppression; without
# it --fatal-warnings converts the intentional layout into a failed build.
ONEROM_ROOT ?= ../../../.work/official-v0.8.1/one-rom-v0.8.1
ORA_DIR := $(ONEROM_ROOT)/firmware/ora
ORA_INCLUDE := -I$(ORA_DIR)
PLUGIN_INCLUDE := -I.

# Use the dependency checkouts made by the official v0.8.1 USB plugin.  The
# official plugin owns their pinned revisions and clones them on its first
# build; sharing those directories prevents this Control fork from silently
# selecting a different USB stack.
TINYUSB_DIR      ?= $(ONEROM_ROOT)/plugins/system/usb/tinyusb
PICOBOOTX_DIR    ?= $(ONEROM_ROOT)/plugins/system/usb/picobootx
PICO_SDKLESS_DIR ?= $(ONEROM_ROOT)/plugins/system/usb/pico-sdkless

BUILD_DIR := build

# The source tree is built both under POSIX and native Windows.  GNU Make's
# Windows build cannot execute POSIX `mkdir -p`/`rm -rf` recipes directly, so
# keep the toolchain invocation identical but use native directory commands.
ifeq ($(OS),Windows_NT)
RWX_SEGMENT_FLAG ?= -Wl,--no-warn-rwx-segments
SHELL := cmd.exe
.SHELLFLAGS := /C
MKDIR_P = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
RM_RF = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
else
RWX_SEGMENT_FLAG ?= -Wl,--no-warn-rwx-segments
MKDIR_P = mkdir -p $(1)
RM_RF = rm -rf $(1)
endif

ifeq ($(wildcard $(TINYUSB_DIR)/src/tusb.c),)
$(error Missing tracked TinyUSB source at $(TINYUSB_DIR))
endif
include tinyusb.mk

ifeq ($(wildcard $(PICOBOOTX_DIR)/picobootx.mk),)
$(error Missing tracked picobootx source at $(PICOBOOTX_DIR))
endif
ifeq ($(wildcard $(PICO_SDKLESS_DIR)/include/pico.h),)
$(error Missing tracked pico-sdkless source at $(PICO_SDKLESS_DIR))
endif
include $(PICOBOOTX_DIR)/picobootx.mk
PICOBOOTX_SRC_C := $(addprefix $(PICOBOOTX_DIR)/,$(PICOBOOTX_SRC_C))
PICOBOOTX_INCLUDE_DIRS := $(addprefix $(PICOBOOTX_DIR)/,$(PICOBOOTX_INCLUDE_DIRS))

PLUGIN_BASE    := 0x10010000
PLUGIN_VERSION := v$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION).$(WORK_BUILD)
PLUGIN_PREFIX  := 1541-OneROM-1541HUD-Monitor-USB-$(PLUGIN_VERSION)

ELF := $(BUILD_DIR)/$(PLUGIN_PREFIX).elf
BIN := $(BUILD_DIR)/$(PLUGIN_PREFIX).bin

SRC := src/usb_descriptors.c src/usb_picobootx.c src/usb_rom.c src/monitor_transport.c src/monitor_protocol.c src/usb_led.c src/usb_gpio.c src/usb_log.c src/usb_main.c

CFLAGS := -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 \
          -O2 -Wall -Wextra -Werror \
          -ffunction-sections -fdata-sections \
          -MMD -MP -g \
		  -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) \
		  -DPATCH_VERSION=$(PATCH_VERSION) -DBUILD_VERSION=$(BUILD_VERSION) \
		   \
		  $(ORA_INCLUDE) $(PLUGIN_INCLUDE) \
		  -Iinclude -I$(PICO_SDKLESS_DIR)/include \
          $(addprefix -I,$(TINYUSB_INCLUDE_DIRS)) \
		  $(addprefix -I,$(PICOBOOTX_INCLUDE_DIRS)) \
		  -std=c11 \
		  $(EXTRA_CFLAGS)

LDFLAGS := -nostdlib \
           -L $(ORA_DIR) \
           -T $(ORA_DIR)/plugin.ld \
           -Wl,--defsym,PLUGIN_TYPE=0 \
           -Wl,--fatal-warnings \
		   $(RWX_SEGMENT_FLAG) \
           -Wl,-Map=$(BUILD_DIR)/$(PLUGIN_PREFIX).map

PLUGIN_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
TINYUSB_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(TINYUSB_SRC_C))
PICOBOOTX_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(PICOBOOTX_SRC_C))
ALL_OBJ     := $(PLUGIN_OBJ) $(TINYUSB_OBJ) $(PICOBOOTX_OBJ)

.PHONY: all clean distclean

all: $(BIN)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@$(call MKDIR_P,$(dir $@))
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(ALL_OBJ) $(ORA_DIR)/plugin.h $(ORA_DIR)/api.h $(ORA_DIR)/system.h $(ORA_DIR)/plugin.ld
	@echo "Linking..."
	@$(CC) $(CFLAGS) $(LDFLAGS) $(ALL_OBJ) -lc -lgcc -o $@

$(BIN): $(ELF)
	@echo "Converting ELF to binary..."
	@$(OBJCOPY) -O binary $< $@
	@$(OBJDUMP) -s -j .plugin_header $(ELF) > $(BUILD_DIR)/$(PLUGIN_PREFIX).dis
	@$(OBJDUMP) -d -S $(ELF) >> $(BUILD_DIR)/$(PLUGIN_PREFIX).dis

$(BUILD_DIR):
	@$(call MKDIR_P,$@)

# ---------------------------------------------------------------------------
# Host build
#
# Compiles the plugin natively so it can be run against the firmware emulator
# for regression testing — see rust/usb-tester.  This produces object files, not
# a plugin binary: there is no linker script and no plugin header placement, and
# the harness supplies the linker symbols and the emulation seams (ORA_SECTION
# and friends are no-ops under ORA_HOST_TEST).
#
# The flags here must match the firmware's own native test build
# (firmware/test.mk), because the two link together:
# - -fshort-enums, or ora_result_t is a different width on each side.
# - -DTEST_BUILD=1, which selects REAL_HARDWARE 0 and drops the metadata layout
#   STATIC_ASSERTs, which assert device offsets a 64-bit host cannot satisfy.
#
# tinyusb and picobootx are on the include path but neither is compiled: the
# plugin only needs their headers to declare what it calls, and the harness
# supplies the handful of entry points behind them.
# ---------------------------------------------------------------------------

HOST_CC        ?= cc
HOST_BUILD_DIR ?= build-host
HOST_OBJ       := $(patsubst src/%.c,$(HOST_BUILD_DIR)/%.o,$(SRC))

# Extra flags for the host objects, and the directory they land in.  A
# coverage build sets both: --coverage here, and a build directory of its own.
# The two must not share a directory, because nothing here compares the flags
# an existing object was built with - a coverage run that reused
# uninstrumented objects would report no coverage rather than fail.
HOST_EXTRA_CFLAGS ?=

HOST_CFLAGS := -DORA_HOST_TEST=1 -DTEST_BUILD=1 -fshort-enums \
               -O1 -g -Wall -Wextra -Werror \
               -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) \
               -DPATCH_VERSION=$(PATCH_VERSION) -DBUILD_VERSION=$(BUILD_VERSION) \
		       $(ORA_INCLUDE) $(PLUGIN_INCLUDE) \
               -Iinclude -I$(PICO_SDKLESS_DIR)/include \
               $(addprefix -I,$(TINYUSB_INCLUDE_DIRS)) \
               $(addprefix -I,$(PICOBOOTX_INCLUDE_DIRS)) \
               -std=c11 -MMD -MP $(HOST_EXTRA_CFLAGS)

.PHONY: host clean-host

host: $(HOST_OBJ)

$(HOST_BUILD_DIR):
	@$(call MKDIR_P,$@)

$(HOST_BUILD_DIR)/%.o: src/%.c | $(HOST_BUILD_DIR)
	@echo "Compiling $< for host..."
	@$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

clean-host:
	@$(call RM_RF,$(HOST_BUILD_DIR))

clean: clean-host
	@echo "Cleaning build artifacts..."
	@$(call RM_RF,$(BUILD_DIR))

distclean: clean
	@echo "Removing cloned repositories..."
	@$(call RM_RF,$(TINYUSB_DIR))
	@$(call RM_RF,$(PICOBOOTX_DIR))
	@$(call RM_RF,$(PICO_SDKLESS_DIR))

# The host objects are listed too, or a header change would rebuild nothing on
# the host side and a test would go on asserting against the last build.
-include $(ALL_OBJ:.o=.d) $(HOST_OBJ:.o=.d)
