
TARGET ?= pico

CURRENT_DIR = $(shell pwd)

DL_DIR = $(CURRENT_DIR)/dl
SRC_DIR = $(CURRENT_DIR)/src
OUTPUT_DIR = $(CURRENT_DIR)/output

# micropython (https://micropython.org/)
MICROPY_VER = 1.19
MICROPY_URL = https://github.com/micropython/micropython/releases/download/v$(MICROPY_VER)/micropython-$(MICROPY_VER).tar.xz
MICROPY_FILE = micropython-$(MICROPY_VER).tar.xz
MICROPY_TXZ = $(DL_DIR)/$(MICROPY_FILE)
MICROPY_SRC = $(SRC_DIR)/micropython

PATCHFILES := $(sort $(wildcard patches/*.patch ))

PATCH_CMD = \
  for f in $(PATCHFILES); do\
      echo $$f ":"; \
      patch -b -p1 < $$f || exit 1; \
  done

COPY_CMD = tar cf - -C files . | tar xf - -C $(SRC_DIR)

.PHONY: all
all: .stamp_output

.stamp_output: .stamp_build
	mkdir -p $(OUTPUT_DIR)
	cp $(MICROPY_SRC)/ports/rp2/build-PICO/firmware.* $(OUTPUT_DIR)
	touch $@

.stamp_build: .stamp_src
	make -C $(MICROPY_SRC)/mpy-cross
	make -C $(MICROPY_SRC)/ports/rp2
	touch $@

.PHONY: clean
clean:
	-rm -rf $(SRC_DIR) .stamp_src
	-rm -rf $(OUTPUT_DIR) .stamp_output
	-rm -rf .stamp_build

$(MICROPY_TXZ):
	mkdir -p $(DL_DIR)
	wget $(MICROPY_URL) -O $(MICROPY_TXZ)

.stamp_src: $(MICROPY_TXZ) $(PATCHFILES)
	mkdir -p $(MICROPY_SRC)
	tar -C $(MICROPY_SRC) -Jxf $(MICROPY_TXZ) --strip-components 1
	$(COPY_CMD)
	$(PATCH_CMD)
	touch $@
