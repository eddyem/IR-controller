BIN=testproject

# Headers that don't have c-files with same name
#  but project depends on them
INDEPENDENT_HEADERS=usb_conf.h includes.h

# Here are STDPeriphLib objects used in project
SPL=
SPL+=misc
SPL+=stm32f10x_adc
SPL+=stm32f10x_bkp
SPL+=stm32f10x_can
SPL+=stm32f10x_cec
SPL+=stm32f10x_crc
SPL+=stm32f10x_dac
SPL+=stm32f10x_dbgmcu
SPL+=stm32f10x_dma
SPL+=stm32f10x_exti
SPL+=stm32f10x_flash
SPL+=stm32f10x_fsmc
SPL+=stm32f10x_gpio
SPL+=stm32f10x_i2c
SPL+=stm32f10x_iwdg
SPL+=stm32f10x_pwr
SPL+=stm32f10x_rcc
SPL+=stm32f10x_rtc
SPL+=stm32f10x_sdio
SPL+=stm32f10x_spi
SPL+=stm32f10x_tim
SPL+=stm32f10x_usart
SPL+=stm32f10x_wwdg

STLIBDIR=stm32_lib
OBJDIR=mk
USB_LIBDIR=usb_lib

INDEPENDENT_HEADERS+=$(STLIBDIR)/stm32f10x_conf.h

TOOLS_PATH=/usr/arm-none-eabi
TOOLS_PREFIX=arm-none-eabi-

CPFLAGS=-Obinary
ODFLAGS=-S

CFLAGS=-c -mcpu=cortex-m3 -mthumb -Wall -Werror -mapcs-frame -D__thumb2__=1 -Os
CFLAGS+=-msoft-float -gdwarf-2 -mno-sched-prolog -fno-hosted -mtune=cortex-m3
CFLAGS+=-march=armv7-m -mfix-cortex-m3-ldrd -ffunction-sections -fdata-sections
CFLAGS+=-I./cmsis -I./stm32_lib -I./usb_lib -I.
CFLAGS+=-DSTM32F10X_MD

LDFLAGS=-static -mcpu=cortex-m3 -mthumb -mthumb-interwork -Wl,--start-group
LDFLAGS+=-L$(TOOLS_PATH)/lib/thumb2 -lc -lg -lstdc++ -lsupc++ -lgcc -lm
LDFLAGS+=-Wl,--end-group -Xlinker -Map -Xlinker $(BIN).map -Xlinker
LDFLAGS+=-T ./cmsis/stm32f100rb_flash.ld -o $(BIN)
LDFLAGS+=-Wl,--gc-sections -Wl,-s

ASFLAGS=-mcpu=cortex-m3 -I./cmsis -I./stm32_lib -gdwarf-2 -gdwarf-2

CC=$(TOOLS_PREFIX)gcc
AS=$(TOOLS_PREFIX)as
SIZE=$(TOOLS_PREFIX)size
CP=$(TOOLS_PREFIX)objcopy
OD=$(TOOLS_PREFIX)objdump

# These are main project sources
SRC=$(wildcard *.c)
MAIN_OBJ=$(addprefix $(OBJDIR)/, $(SRC:%.c=%.o))

STM32_LIBSRC=$(addprefix $(STLIBDIR)/, $(addsuffix .c, $(SPL)))
OBJ=$(addprefix $(OBJDIR)/, $(addsuffix .o, $(SPL)))

OBJ+=$(OBJDIR)/system_stm32f10x.o $(OBJDIR)/startup.o
STM32_LIBSRC+=cmsis/system_stm32f10x.c

# USB library sources
USB_SRC=$(wildcard $(USB_LIBDIR)/*.c)
USB_OBJ=$(addprefix $(OBJDIR)/, $(USB_SRC:$(USB_LIBDIR)/%.c=%.o))

ALL_OBJ=$(OBJ) $(MAIN_OBJ) $(USB_OBJ)
ALL_SRC=$(SRC) $(STM32_LIBSRC) $(USB_SRC)

all: $(BIN)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: $(STLIBDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/system_stm32f10x.o: ./cmsis/system_stm32f10x.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/startup.o: ./cmsis/startup_stm32f10x_md.s
	$(AS) $(ASFLAGS) ./cmsis/startup_stm32f10x_md.s -o $(OBJDIR)/startup.o

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: $(USB_LIBDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(SRC) : %.c : %.h $(INDEPENDENT_HEADERS)
	@touch $@

%.h: ;

$(BIN): $(OBJDIR) $(ALL_OBJ)
	$(CC) $(ALL_OBJ) $(LDFLAGS)
	$(CP) $(CPFLAGS) $(BIN) $(BIN).bin
	$(OD) $(ODFLAGS) $(BIN) > $(BIN).list
	$(SIZE) -B $(BIN)

clean:
	rm -f $(ALL_OBJ) $(BIN).map $(BIN) $(BIN).list
	rmdir $(OBJDIR)

load: $(BIN)
	st-flash write $(BIN).bin 0x08000000


.PHONY: clean load
