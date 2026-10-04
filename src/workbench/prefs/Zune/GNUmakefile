# Native GNU build; the SAS/C smakefile remains independent.
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
CROSS ?= m68k-amigaos-
CC := $(CROSS)gcc
BUILD ?= ../../../../build/muimaster
CPUFLAGS ?= -m68000 -msoft-float
CFLAGS ?= -Os -g -Wall -Wno-pointer-sign
MASTER := ../../libs/muimaster
CPPFLAGS += -DNO_INLINE_STDARG -I$(BUILD)/include \
  -Iinclude -I$(MASTER)/include -include clib/alib_protos.h
SOURCES := zune.c listview.c zunestuff.c windowp.c buttonsp.c groupsp.c \
  cyclesp.c slidersp.c scrollbarsp.c listviewsp.c stringsp.c specialp.c \
  navigationp.c framesp.c locale.c systemp.c amigasupport.c
OBJECTS := $(addprefix $(BUILD)/prefs/,$(SOURCES:.c=.o))
.PHONY: all
all: $(BUILD)/Prefs/Zune
$(BUILD)/Prefs/Zune: $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CPUFLAGS) -noixemul $(LDFLAGS) -Wl,-u,___stkinit -o $@ $^ -lmui -ldebug
$(BUILD)/prefs/%.o: %.c GNUmakefile
	@mkdir -p $(@D)
	$(CC) $(CPUFLAGS) -noixemul $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@
-include $(OBJECTS:.o=.d)
