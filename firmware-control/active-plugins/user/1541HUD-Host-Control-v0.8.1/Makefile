# Copyright (C) 2026 Piers Finlayson <piers@piers.rocks>
#
# MIT License

TOOLCHAIN ?=
CC      := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-gcc,arm-none-eabi-gcc)
OBJCOPY := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-objcopy,arm-none-eabi-objcopy)
OBJDUMP := $(if $(TOOLCHAIN),$(TOOLCHAIN)/arm-none-eabi-objdump,arm-none-eabi-objdump)

# GNU ld 2.39+ reports the deliberately flat plugin segment as RWX.  Older
# supported toolchains do not recognise the suppression, so leave it opt-in.
# Support the recovered build on native Windows too.  The toolchain itself is
# already native; only the original POSIX directory-management recipes differ.
ifeq ($(OS),Windows_NT)
RWX_SEGMENT_FLAG ?= -Wl,--no-warn-rwx-segments
SHELL := cmd.exe
.SHELLFLAGS := /C
MKDIR_P = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
RM_RF = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
else
RWX_SEGMENT_FLAG ?=
MKDIR_P = mkdir -p $(1)
RM_RF = rm -rf $(1)
endif

MAJOR_VERSION := 0
MINOR_VERSION := 8
PATCH_VERSION := 1
BUILD_VERSION := 3

# Maximum size of the flash erase routine.  This is copied onto the stack at
# NV commit time.  Used to size the buffer and checked at link time.
NV_ERASE_FN_MAX := 80

# Maximum bytes of static data (ring buffer, .ramfunc, .data, .bss). The
# remainder of a user plugin's static RAM is stack.
# Increasing this required explicit sign off.
# Four bytes accommodate the SEL_A IEC 8/9 boot overlay while preserving the
# v8.1 host-control plugin's static-RAM guardrail.
PLUGIN_STATIC_MAX := 424

ONEROM_ROOT ?= ../../../source/control-baseline-v0.8.1-release
ORA_DIR := $(ONEROM_ROOT)/firmware/ora
ORA_INCLUDE := -I$(ORA_DIR)
PLUGIN_INCLUDE := -I.

BUILD_DIR := build

PLUGIN_PREFIX := 1541-OneROM-1541HUD-Host-Control-v$(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION).$(BUILD_VERSION)

ELF := $(BUILD_DIR)/$(PLUGIN_PREFIX).elf
BIN := $(BUILD_DIR)/$(PLUGIN_PREFIX).bin

SRC := src/host_control_main.c

FLASH_ERASE_SRC := src/flash_erase.c
FLASH_ERASE_OBJ := $(BUILD_DIR)/src/flash_erase.o
FLASH_ERASE_EXTRA_CFLAGS := -fPIC -fno-plt

CFLAGS := -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 \
          -fstack-usage -fconserve-stack \
          -O3 -Wall -Wextra -Werror \
          -ffunction-sections -fdata-sections \
          -MMD -MP \
		  -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) \
		  -DPATCH_VERSION=$(PATCH_VERSION) -DBUILD_VERSION=$(BUILD_VERSION) \
		  -DNV_ERASE_FN_MAX=$(NV_ERASE_FN_MAX)u \
          $(ORA_INCLUDE) $(PLUGIN_INCLUDE) \
		  -Iinclude

LDFLAGS := -nostdlib \
           -L $(ORA_DIR) -T host_control_plugin.ld \
           -Wl,--defsym=NV_ERASE_FN_MAX=$(NV_ERASE_FN_MAX) \
           -Wl,--defsym=PLUGIN_STATIC_MAX=$(PLUGIN_STATIC_MAX) \
           -Wl,--fatal-warnings \
		   $(RWX_SEGMENT_FLAG) \
           -Wl,-Map=$(BUILD_DIR)/$(PLUGIN_PREFIX).map

PLUGIN_OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
ALL_OBJ    := $(PLUGIN_OBJ) $(FLASH_ERASE_OBJ)

# ---------------------------------------------------------------------------
# Host build
#
# Compiles the plugin natively so it can be run against the firmware emulator
# for regression testing — see rust/plugin-tester.  This produces an object
# file, not a plugin binary: there is no linker script and no plugin header
# placement, and the harness supplies the linker symbols and the emulation
# seams (ORA_SECTION and friends are no-ops under ORA_HOST_TEST).
#
# -fshort-enums must match the firmware's own native test build
# (firmware/test.mk) because the two link together, or ora_result_t is a
# different width on each side.
#
# -Werror matches the ARM build.  The two casts in the NV commit path that used
# to warn on a 64-bit host — the flash offset computed from __nv_storage_start,
# and the erase routine's address formed from a RAM slot base with the Thumb
# bit set — now go through ORA_FLASH_OFFSET and ORA_STAGED_FN_PTR.
# ---------------------------------------------------------------------------

HOST_CC          ?= cc
HOST_BUILD_DIR   ?= build-host
HOST_OBJ         := $(HOST_BUILD_DIR)/host_control_main.o
HOST_FLASH_ERASE := $(HOST_BUILD_DIR)/flash_erase.o
HOST_ALL_OBJ     := $(HOST_OBJ) $(HOST_FLASH_ERASE)

# Extra flags for the host objects, and the directory they land in.  A
# coverage build sets both: --coverage here, and a build directory of its own.
# The two must not share a directory, because nothing here compares the flags
# an existing object was built with - a coverage run that reused
# uninstrumented objects would report no coverage rather than fail.
HOST_EXTRA_CFLAGS ?=

HOST_CFLAGS := -DORA_HOST_TEST=1 -fshort-enums \
               -O1 -g -Wall -Wextra -Werror \
               -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) \
               -DPATCH_VERSION=$(PATCH_VERSION) -DBUILD_VERSION=$(BUILD_VERSION) \
               -DNV_ERASE_FN_MAX=$(NV_ERASE_FN_MAX)u \
               $(ORA_INCLUDE) $(PLUGIN_INCLUDE) \
               -Iinclude \
               -MMD -MP $(HOST_EXTRA_CFLAGS)

.PHONY: all clean distclean host clean-host

all: $(BIN)

host: $(HOST_ALL_OBJ)

$(HOST_BUILD_DIR):
	@$(call MKDIR_P,$@)

$(HOST_OBJ): $(SRC) | $(HOST_BUILD_DIR)
	@echo "Compiling $< for host..."
	@$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

# The erase routine is compiled for the host too, and the harness calls the
# real thing rather than the bytes the plugin staged — see ORA_STAGED_FN_PTR.
# So the sequence under test is the plugin's own, not a re-description of it.
$(HOST_FLASH_ERASE): $(FLASH_ERASE_SRC) | $(HOST_BUILD_DIR)
	@echo "Compiling $< for host..."
	@$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

clean-host:
	@$(call RM_RF,$(HOST_BUILD_DIR))

$(FLASH_ERASE_OBJ): $(FLASH_ERASE_SRC) | $(BUILD_DIR)
	@$(call MKDIR_P,$(dir $@))
	@echo "Compiling $< (PIC)..."
	@$(CC) $(CFLAGS) $(FLASH_ERASE_EXTRA_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@$(call MKDIR_P,$(dir $@))
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(ALL_OBJ) $(ORA_DIR)/plugin.h $(ORA_DIR)/api.h $(ORA_DIR)/system.h host_control_plugin.ld $(ORA_DIR)/onerom_linker_constants_generated.ld
	@echo "Linking..."
	@$(CC) $(CFLAGS) $(LDFLAGS) $(ALL_OBJ) -lc -lgcc -o $@

$(BIN): $(ELF)
	@echo "Converting ELF to binary..."
	@$(OBJCOPY) -O binary $< $@
	@$(OBJDUMP) -s -j .plugin_header $(ELF) > $(BUILD_DIR)/$(PLUGIN_PREFIX).dis
	@$(OBJDUMP) -d -S $(ELF) >> $(BUILD_DIR)/$(PLUGIN_PREFIX).dis

$(BUILD_DIR):
	@$(call MKDIR_P,$@)

clean: clean-host
	@echo "Cleaning build artifacts..."
	@$(call RM_RF,$(BUILD_DIR))

distclean: clean

# The host objects are listed too, or a header change would rebuild nothing on
# the host side and a test would go on asserting against the last build.
-include $(ALL_OBJ:.o=.d) $(HOST_ALL_OBJ:.o=.d)
