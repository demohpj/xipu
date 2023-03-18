UROM_DIR = ../urom
ASM_DIR = ../asm
EMU_DIR = ../emu

ifeq ($(OS),Windows_NT)
	UROM_EXE = urom.exe
	ASM_EXE = asm.exe
	EMU_EXE = emu.exe
	
	LIB_DIR = ../lib
else
	UROM_EXE = urom
	ASM_EXE = asm
	EMU_EXE = emu
endif

ROM_DIR = ../rom
ROM_BIN = rom.bin
ROM_MAP = rom.map

FS_DIR = ../fs
FS_APP = app
FS_FILE = file

#IO_SRC_DIR = ../io/firmware/src
#IO_HEX = io.hex

.PHONY: all
all: urom rom fs io

urom: tools
	rm -rf urom
	mkdir -p urom

ifeq ($(OS),Windows_NT)
ifdef WINE
	cd urom; \
	$(WINE) ../tools/$(UROM_EXE)
else
	cd urom; \
	../tools/$(UROM_EXE)
endif
else
	cd urom; \
	../tools/$(UROM_EXE)
endif

rom: tools
	rm -rf rom
	$(MAKE) -C $(ROM_DIR)
	mkdir -p rom
	cp $(ROM_DIR)/$(ROM_BIN) rom
	cp $(ROM_DIR)/$(ROM_MAP) rom

fs: tools rom
	rm -rf fs
	$(MAKE) -C $(FS_DIR)
	mkdir -p fs
	cp -r $(FS_DIR)/$(FS_APP) fs
	cp -r $(FS_DIR)/$(FS_FILE) fs

io:
	#rm -rf io
	#$(MAKE) -C $(IO_SRC_DIR)
	#mkdir -p io
	#cp $(IO_SRC)/$(IO_HEX) $(IO_DST)/

tools: tools_urom tools_asm tools_emu
	rm -rf tools
	mkdir -p tools
	cp $(UROM_DIR)/src/$(UROM_EXE) tools
	cp $(ASM_DIR)/src/$(ASM_EXE) tools
	cp $(EMU_DIR)/src/$(EMU_EXE) tools
	
ifeq ($(OS),Windows_NT)
	cp -r $(LIB_DIR)/* tools
endif

.PHONY: tools_asm
tools_asm:
	$(MAKE) -C $(ASM_DIR) src

.PHONY: tools_emu
tools_emu:
	$(MAKE) -C $(EMU_DIR) src

.PHONY: tools_urom
tools_urom:
	$(MAKE) -C $(UROM_DIR) src

.PHONY: clean
clean:
	$(MAKE) -C $(ROM_DIR) clean
	$(MAKE) -C $(UROM_DIR) clean
	$(MAKE) -C $(ASM_DIR) clean
	$(MAKE) -C $(EMU_DIR) clean
	$(MAKE) -C $(FS_DIR) clean
	
	rm -rf tools
	rm -rf urom
	rm -rf rom
	rm -rf fs
	rm -rf io
