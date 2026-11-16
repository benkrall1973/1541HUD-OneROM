CA65 ?= ca65
LD65 ?= ld65
PYTHON ?= python3
OUT ?= ../../../../roms/private/s-jiffydos-autostart
RUNTIME_FLASH_SLOT ?= 10

.PHONY: all test clean
all: $(OUT)/s-jiffydos-autostart-e000-ffff.bin

build:
	mkdir -p build

build/autostart.o: autostart.s | build
	$(CA65) -g -D RBCP_RUNTIME_FLASH_SLOT=$(RUNTIME_FLASH_SLOT) -o $@ $<

$(OUT)/s-jiffydos-autostart-e000-ffff.bin: build/autostart.o autostart.cfg | build
	mkdir -p $(OUT)
	$(LD65) -C autostart.cfg -o $@ -m build/autostart.map -Ln build/autostart.lbl $<

test: all
	AUTOSTART_IMAGE="$(OUT)/s-jiffydos-autostart-e000-ffff.bin" AUTOSTART_RUNTIME_FLASH_SLOT=$(RUNTIME_FLASH_SLOT) $(PYTHON) test_autostart.py

clean:
	rm -rf build
	rm -f $(OUT)/s-jiffydos-autostart-e000-ffff.bin
