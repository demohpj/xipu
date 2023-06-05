UROM_DOC_DIR = ../urom/doc
ASM_DOC_DIR = ../asm/doc
EMU_DOC_DIR = ../emu/doc
FW_DOC_DIR = ../fw/doc

.PHONY: all
all: urom asm emu fw

.PHONY: urom
urom:
	rm -rf urom
	$(MAKE) -C $(UROM_DOC_DIR)
	cp -R $(UROM_DOC_DIR)/html urom

.PHONY: asm
asm:
	rm -rf asm
	$(MAKE) -C $(ASM_DOC_DIR)
	cp -R $(ASM_DOC_DIR)/html asm

.PHONY: emu
emu:
	rm -rf emu
	$(MAKE) -C $(EMU_DOC_DIR)
	cp -R $(EMU_DOC_DIR)/html emu

.PHONY: fw
fw:
	rm -rf fw
	$(MAKE) -C $(FW_DOC_DIR)
	cp -R $(FW_DOC_DIR)/html fw

.PHONY: clean
clean:
	$(MAKE) -C $(UROM_DOC_DIR) clean
	$(MAKE) -C $(ASM_DOC_DIR) clean
	$(MAKE) -C $(EMU_DOC_DIR) clean
	$(MAKE) -C $(FW_DOC_DIR) clean
	
	rm -rf urom
	rm -rf asm
	rm -rf emu
	rm -rf fw
