
# host compilation tools
HOST_GCC = gcc

# cross compilation tools

# set the path
XTOOLS_DIR = /opt/gcc-arm-none-eabi-10.3-2021.10

# libgcc path (soft float)
X_LIBGCC_DIR = $(XTOOLS_DIR)/lib/gcc/arm-none-eabi/10.3.1/arm/v5te/softfp

# libc path (soft float)
X_LIBC_DIR = $(XTOOLS_DIR)/arm-none-eabi/lib/arm/v5te/softfp

X_GCC = $(XTOOLS_DIR)/bin/arm-none-eabi-gcc
X_OBJCOPY = $(XTOOLS_DIR)/bin/arm-none-eabi-objcopy
X_AR = $(XTOOLS_DIR)/bin/arm-none-eabi-ar
X_LD = $(XTOOLS_DIR)/bin/arm-none-eabi-ld
X_GDB = $(XTOOLS_DIR)/bin/arm-none-eabi-gdb

# cross compile flags (cortex m0+)
X_CFLAGS = -Werror -Wall -Wextra -Wstrict-prototypes
X_CFLAGS += -O2
X_CFLAGS += -falign-functions -fomit-frame-pointer -fno-strict-aliasing
X_CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0plus -mthumb-interwork
X_CFLAGS += -std=c99
