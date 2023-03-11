TOP = .
include $(TOP)/mk/common.mk

TARGET?=atsaml21_xpro

TARGET_DIR = $(TOP)/target/$(TARGET)
BIN_FILE = $(TARGET_DIR)/opengate.bin

.PHONY: all program clean

all:
	make -C $(TARGET_DIR) $@

clean:
	make -C $(TARGET_DIR) $@
