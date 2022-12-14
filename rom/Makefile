ifeq ($(OS),Windows_NT)
ifdef WINE
	ASM = $(WINE) ../bin/tools/asm.exe
else
	ASM = ../bin/tools/asm.exe
endif
else
	ASM = ../bin/tools/asm
endif

.PHONY: all
all: rom.bin

rom.bin: src/*.asm
	$(ASM) rom src/rom.asm rom.bin rom.map

.PHONY: clean
clean:
	rm -f rom.bin
	rm -f rom.map
