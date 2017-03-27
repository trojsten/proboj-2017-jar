
all clean:
	$(foreach file,$(wildcard */Makefile) $(wildcard */*/Makefile),$(MAKE) -C $(dir $(file)) $@ &&) true

