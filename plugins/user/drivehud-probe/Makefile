PLUGIN_NAME := user/drivehud-probe-v001
PLUGIN_SRC := src/drivehud_probe_v001.c
PLUGIN_TYPE := USER
BUILD_DIR := build
ORA_DIR := ../../../firmware/ora
PLUGIN_MK := $(ORA_DIR)/plugin.mk

.PHONY: all plugin clean
all: plugin
plugin:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -f $(PLUGIN_MK) \
		SRC=$(CURDIR)/$(PLUGIN_SRC) \
		BUILD_DIR=$(CURDIR)/$(BUILD_DIR) \
		ORA_INCLUDE=$(CURDIR)/$(ORA_DIR) \
		PLUGIN_TYPE=$(PLUGIN_TYPE) \
		TOOLCHAIN=$(TOOLCHAIN)
clean:
	rm -rf $(BUILD_DIR)
