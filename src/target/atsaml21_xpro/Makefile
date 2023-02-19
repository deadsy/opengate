TOP = ../..
include $(TOP)/mk/common.mk

OUTPUT = opengate

CHIP=saml21j18a

# soc sources
SOC_DIR = $(TOP)/soc/microchip/saml21a1
SRC += $(SOC_DIR)/gcc/gcc/startup_$(CHIP).c \
       $(SOC_DIR)/gcc/system_$(CHIP).c  \

# target sources
TARGET_DIR = $(TOP)/target/atsaml21_xpro
SRC += $(TARGET_DIR)/main.c \

OBJ = $(patsubst %.c, %.o, $(SRC))

# include paths
INCLUDE += -I$(TARGET_DIR)
INCLUDE += -I$(SOC_DIR)/include
INCLUDE += -I$(TOP)/soc/microchip/cmsis/Include

# linker flags
LDSCRIPT = $(SOC_DIR)/gcc/gcc/$(CHIP)_flash.ld 
X_LDFLAGS = -T$(LDSCRIPT) -Wl,-Map,$(OUTPUT).map -Wl,--gc-sections

.c.o:
	$(X_GCC) $(INCLUDE) $(DEFINE) $(X_CFLAGS) -c $< -o $@

.PHONY: all clean

all: $(OBJ)
	$(X_GCC) $(X_CFLAGS) $(X_LDFLAGS) $(OBJ) -o $(OUTPUT)
	$(X_OBJCOPY) -O binary $(OUTPUT) $(OUTPUT).bin

clean:
	-rm $(OBJ)	
	-rm $(OUTPUT)
	-rm $(OUTPUT).map	
	-rm $(OUTPUT).bin	
