TOP = ../..
include $(TOP)/mk/common.mk

OUTPUT = opengate

# soc sources
SOC_DIR = $(TOP)/soc/microchip/samd21a
SRC += $(SOC_DIR)/gcc/gcc/startup_samd21g18a.c \
       $(SOC_DIR)/gcc/system_samd21g18a.c  \

# target sources
TARGET_DIR = $(TOP)/target/maduino_zero
SRC += $(TARGET_DIR)/main.c \

OBJ = $(patsubst %.c, %.o, $(SRC))

# include paths
INCLUDE += -I$(TARGET_DIR)
INCLUDE += -I$(SOC_DIR)/include
INCLUDE += -I$(TOP)/soc/microchip/cmsis/Include

# linker flags
LDSCRIPT = $(SOC_DIR)/gcc/gcc/samd21g18a_flash.ld 
X_LDFLAGS = -T$(LDSCRIPT) -Wl,-Map,$(OUTPUT).map -Wl,--gc-sections

X_CFLAGS += -Wall
X_CFLAGS += -Wstrict-prototypes

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
