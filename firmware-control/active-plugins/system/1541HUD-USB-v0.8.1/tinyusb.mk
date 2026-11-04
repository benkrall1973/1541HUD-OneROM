
# C source files
TINYUSB_SRC_C += \
	$(TINYUSB_DIR)/src/tusb.c \
	$(TINYUSB_DIR)/src/common/tusb_fifo.c \
	$(TINYUSB_DIR)/src/device/usbd.c \
	$(TINYUSB_DIR)/src/device/usbd_control.c \
	$(TINYUSB_DIR)/src/class/cdc/cdc_device.c \
	$(TINYUSB_DIR)/src/portable/raspberrypi/rp2040/dcd_rp2040.c \
	$(TINYUSB_DIR)/src/portable/raspberrypi/rp2040/rp2040_usb.c

TINYUSB_INCLUDE_DIRS += \
	$(TINYUSB_DIR)/src \
	$(TINYUSB_DIR)/src/common \
	$(TINYUSB_DIR)/src/device \
	$(TINYUSB_DIR)/src/class/cdc \
	$(TINYUSB_DIR)/src/portable/raspberrypi/rp2040
