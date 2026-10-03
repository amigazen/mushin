# GNU make entry point; SAS/C builds keep their smakefiles.
.DEFAULT_GOAL := all
.PHONY: all clean check _forward
# One sub-make owns the dependency graph even with parallel top-level goals.
all clean check: _forward
	@:

_forward:
	$(MAKE) -C src/workbench/libs/muimaster -f GNUmakefile \
	  $(if $(MAKECMDGOALS),$(MAKECMDGOALS),all)
