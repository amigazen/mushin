# GNU make entry point; SAS/C builds keep their smakefiles.
.DEFAULT_GOAL := all
.PHONY: all core prefs plugins catalogs sdk examples clean check check-core check-sdk release _forward
# One sub-make owns the dependency graph even with parallel top-level goals.
all core prefs plugins catalogs sdk examples clean check check-core check-sdk release: _forward
	@:

_forward:
	$(MAKE) -C src/workbench/libs/muimaster -f GNUmakefile \
	  $(if $(MAKECMDGOALS),$(MAKECMDGOALS),all)
