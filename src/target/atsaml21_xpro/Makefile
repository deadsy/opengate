TOP = ../..
include $(TOP)/mk/common.mk

OUTPUT = opengate

CHIP=saml21j18a

# chip sources
CHIP_DIR = $(SOC_DIR)/saml21a1
SRC += $(CHIP_DIR)/gcc/gcc/startup_$(CHIP).c \
       $(CHIP_DIR)/gcc/system_$(CHIP).c  \

# target sources
TARGET_DIR = $(TOP)/target/atsaml21_xpro
SRC += $(TARGET_DIR)/main.c \

# soc sources
SOC_DIR = $(TOP)/soc/microchip
SRC += $(SOC_DIR)/lib/gpio.c \

# common sources
COMMON_DIR = $(TOP)/common
SRC += $(COMMON_DIR)/logging.c \
	$(COMMON_DIR)/debounce.c \
	$(COMMON_DIR)/keyscan.c \
	$(COMMON_DIR)/rtt/SEGGER_RTT.c \
	$(COMMON_DIR)/rtt/SEGGER_RTT_printf.c \

OBJ = $(patsubst %.c, %.o, $(SRC))

# include paths
INCLUDE += -I$(TARGET_DIR)
INCLUDE += -I$(CHIP_DIR)/include
INCLUDE += -I$(SOC_DIR)/cmsis/Include
INCLUDE += -I$(SOC_DIR)/lib
INCLUDE += -I$(COMMON_DIR)
INCLUDE += -I$(COMMON_DIR)/rtt

# linker flags
LDSCRIPT = $(CHIP_DIR)/gcc/gcc/$(CHIP)_flash.ld 
X_LDFLAGS = -T$(LDSCRIPT) -Wl,-Map,$(OUTPUT).map -Wl,--gc-sections

# defines
DEFINE +=-DUSE_CMSIS_INIT
DEFINE +=-D__SAML21J18A__
DEFINE += -DSTDIO_RTT

.c.o:
	$(X_GCC) $(INCLUDE) $(DEFINE) $(X_CFLAGS) -c $< -o $@

.PHONY: all clean

all: $(OBJ)
	$(X_GCC) $(X_CFLAGS) $(X_LDFLAGS) $(OBJ) -o $(OUTPUT)
	$(X_OBJCOPY) -O ihex $(OUTPUT) $(OUTPUT).hex

clean:
	-rm $(OBJ)	
	-rm $(OUTPUT)
	-rm $(OUTPUT).map	
	-rm $(OUTPUT).hex	
