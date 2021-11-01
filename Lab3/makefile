VHD=$(shell ls | grep .vhd)

topmodule: $(VHD)
	ghdl -c --std=08 -fsynopsys $^ -r $@
