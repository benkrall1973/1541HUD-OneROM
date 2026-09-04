PLUGIN_NAME := user/drivehud-probe
PLUGIN_SRC := src/drivehud_probe_v017.c
PLUGIN_TYPE := USER

BUILD_DIR := build
ORA_DIR := ../../../firmware/ora
PLUGIN_MK := $(ORA_DIR)/plugin.mk

.PHONY: all plugin clean distclean build_dir

all: plugin

plugin: build_dir
	@$(MAKE) -f $(PLUGIN_MK) \
		SRC=$(CURDIR)/$(PLUGIN_SRC) \
		BUILD_DIR=$(CURDIR)/$(BUILD_DIR) \
		ORA_INCLUDE=$(CURDIR)/$(ORA_DIR) \
		EXTRA_C_FLAGS="-I$(CURDIR)/src" \
		PLUGIN_TYPE=$(PLUGIN_TYPE) \
		TOOLCHAIN=$(TOOLCHAIN)

build_dir:
	@mkdir -p $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR)

distclean: clean
