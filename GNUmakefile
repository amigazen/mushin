# GNU make entry point; SAS/C builds keep their smakefiles.
.DEFAULT_GOAL := all
.PHONY: all clean check
all clean check:
	$(MAKE) -C src/workbench/libs/muimaster -f GNUmakefile $@
