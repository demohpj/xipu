# XiPU

XiPU is an 8-bit 7400 TTL CPU project.

## 1. About the project

The XiPU is loosely based on MOS 6502. It was my starting point of design my own CPU. For now, the XiPU has many differences and it is so much simpler.

The main goal of the project is creating a fully functional 8-bit 7400 TTL CPU and a simple computer based on the one.

### 1.1. Author

- Pawel Jablonski (pj@xirx.net)

### 1.2. License

You can use this code however you like but leave information about the original author. Code is free for non-commercial and commercial use.

Please see the LICENSE.txt file.

### 1.3. Links

- WWW: [xirx.net](https://xirx.net "xirx.net")
- GIT: [git.xirx.net](https://git.xirx.net "git.xirx.net")

## 2. Useful knowledge

### 2.1. Dictionary

- XiPU - XiRX Processor Unit. The name of the 8-bit TTL CPU.
- XiPC - XiRX Personal Computer. The name of the simple computer based on XiPU CPU
- TTL - Transistor-transistor logic.
- HCT - High-Speed CMOS chips available in TTL compatible form.
- RAM - Volatile random access memory.
- ROM - Non-volatile read-only memory.
- Register - Quickly accessible location for a small amount of data stored in a TTL chip.
- uROM - uROM determines which of the internal registers is used to read from and write to at the current micro step of executing instruction.
- IO operation - Read or write data to pins connected to external world.
- External BUS - Data highway for IO operations.
- ALU - Arithmetic logic unit.

### 2.2. XiPU

- 8-bit processor based on 7400 HCT chips compatible with TTL.
- 32 KB of ROM space for OS. It is stored in EEPROM.
- 64 KB of RAM space. Lower 32 KB is a place for variables and temporal data. Higher 32 KB is visible as higher 32 KB of ROM space, it could be used to load and run applications.
- 1 MHz clock speed. The processor uses a 2 MHz crystal to get two 1 MHz square waves shifted relative to the other about half.
- The processor uses uROM stored in two EEPROMs.
- Each instruction could be built from 2 to 16 micro steps. Each micro step took exactly 1 clock tick.
- Four registers:
	- A, B - 8-bit general purpose registers.
	- X, Y - 8-bit auxiliary registers. Could be used mostly as general purpose registers. Together, it can be used as a 16-bit address register.
- Three internal buses:
	- BUS A - 8-bit main BUS for registers, RAM, ROM, IO operations.
	- BUS B - 8-bit auxiliary BUS for registers used as second input for ALU.
	- BUS C - 16-bit memory address BUS for addressing RAM and ROM.
- 8-bit IN and 8-bit OUT external data BUS. It is used as a simple communication way with the external word.

### 2.3. XiPC

- Personal computer based on the XiPU.
- 40x30 characters with 16 colors support. It uses 640x480 LCD with an 8-bit color palette to render image output.
- Mono speaker with a volume knob.
- RS-232 with 1200 bps support.
- RTC with battery support.
- Built-in QWERTY keyboard.
- 4 status LEDs.
- 7.5V max 0.5A power supply.

## 3. Project overview

### 3.1. Requirements

The project supports building process on Windows and Linux.

- Geany - Multi-language IDE. Used to create Makefile, assembler code etc.
- QtCreator - IDE for C++/Qt. Used to create desktop applications.
- Qt 5.12.4 - Multiplatform Qt 5 library. It supports UI, data structures, files etc.
- GCC - C and C++ code compiler.
- Make - Build automation tool.
- Doxygen 1.9.6 - Documentation generator from annotated sources.
- Proteus 8 - Circuit simulator software. Used to simulate the XiPU.
- QCAD - DXF files editor. Used to create Plexiglas case for the XiPC.
- DipTrace 4.2.0.1 - Schematic and PCB design software.

### 3.2. Project structure

```console
.
├── LICENSE.txt    # License file.
├── Makefile       # Main Makefile file to building binaries and documentations at once.
├── README.md      # Readme file.
├── app            # Applications written for the XiPC.
├── asm
│   ├── doc        # Documentation of the Assembler Compiler to make by Doxygen.
│   └── src        # Source code of the Assembler Compiler.
├── bin            # Destination directory for all generated binaries during the build process.
├── case           # A simple Plexiglas case project for the XiPC.
├── doc            # Destination directory for all generated documentations during the build process.
├── emu
│   ├── doc        # Documentation of the XiPC Emulator to make by Doxygen.
│   └── src        # Source code of the XiPC Emulator.
├── fonts          # Fonts used in desktop GUI applications.
├── fs             # File system generator needed by the real XiPC and the XiPC Emulator.
├── ide
│   ├── geany      # IDE configuration to support assembler for the XiPU.
│   └── qtcreator  # IDE configuration for code rules.
├── io             # PCB design for the IO Board
├── kbd            # PCB design for the keyboard
├── lib            # Binary libraries needed to run compiled applications on Windows x64.
├── pcb            # PCB design for the XiPU
├── rom            # OS with a simple terminal and IO API for applications.
├── sim            # Simulation project of the XiPU.
└── urom
    ├── doc        # Documentation of the uROM generator to make by Doxygen.
    └── src        # Source code of the uROM generator.
```

## 4. Compiling

### 4.1. Build all

To build all binaries and create documentations for them, please type:

```console
make
```

### 4.2. Build a single module

#### 4.2.1 uROM

To build the application and create documentation, please type:

```console
cd urom
make
```

#### 4.2.2. ASM Compiler

To build the application and create documentation, please type:

```console
cd asm
make
```

#### 4.2.3. ROM

To build the OS ROM image file, the map file with API handlers, and create documentation, please type:

```console
cd rom
make
```

#### 4.2.4. Applications

To build the user applications, please type:

```console
cd app
make
```

#### 4.2.5. File System

To generate a directory with the virtual file system, please type:

```console
cd fs
make
```

#### 4.2.6. XiPC Emulator

To build the application and create documentation, please type:

```console
cd emu
make
```

## 5. Using

### 5.1. uROM

To generate uROM files, please type:

```console
./urom
```

### 5.2. ASM Compiler

To compile ROM, please type:

```console
./asm rom [input_file] [output_bin] [output_map]
```

To compile an application, please type:

```console
./asm app [input_file] [output_bin]
```

#### 5.3. XiPC Emulator

To run the XiPC Emulator application, please type:

```console
./emu
```

It will show a window of the emulator ready to work. Next step is loading uROM files, ROM file and choose the directory with the virtual file system. After that, the application is ready to run emulation.
