# XiPU

XiPU is an 8-bit 7400 TTL CPU project.

## About the project

The XiPU is loosely based on MOS 6502. It was my starting point of design my own CPU. For now, the XiPU has many differences and it is so much simpler.

The main goal of the project is creating a fully functional 8-bit 7400 TTL CPU and a simple computer based on the one.

### Author

- Pawel Jablonski (pj@xirx.net)

### License

You can use this code however you like but leave information about the original author. Code is free for non-commercial and commercial use.

Please see the LICENSE.txt file.

### Links

- WWW: [xirx.net](https://xirx.net "xirx.net")
- GIT: [git.xirx.net](https://git.xirx.net "git.xirx.net")

## Useful knowledge

### Dictionary

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

### XiPU

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

### XiPC

- Personal computer based on the XiPU.
- 40x30 characters with 16 colors support. It uses 640x480 LCD with an 8-bit color palette to render image output.
- Mono speaker with a volume knob.
- RS-232 with 1200 bps support.
- RTC with battery support.
- Built-in QWERTY keyboard.
- 4 status LEDs.
- 7.5V max 0.5A power supply.

## Project overview

### Requirements

The project supports building process on Windows and Linux.

- Geany - Multi-language IDE. Used to create Makefile, assembler code etc.
- QtCreator - IDE for C++/Qt. Used to create desktop applications.
- Qt 5.12.4 - Multiplatform Qt 5 library. It supports UI, data structures, files etc.
- GCC - C and C++ code compiler.
- Make - Build automation tool.
- Doxygen 1.9.5 - Documentation generator from annotated sources.
- Proteus 8 - Circuit simulator software. Used to simulate the XiPU.
- QCAD - DXF files editor. Used to create Plexiglas case for the XiPC.

### Project structure

```console
.
├── asm            # Assembler Compiler for the XiPU.
├── case           # A simple Plexiglas case project for the XiPC.
├── ide
│   ├── geany      # IDE configuration to support assembler for the XiPU.
│   └── qtcreator  # IDE configuration for code rules.
├── lib            # Binary libraries needed to run compiled applications on Windows.
├── rom            # OS with a simple terminal and IO API for applications.
├── sim            # Simulation project of the XiPU.
└── urom
    ├── doc        # Documentation of the uROM generator to make by Doxygen.
    └── src        # Source of the uROM generator.
```

## Compiling

### uROM

To build the application and create documentation, please type:

```console
cd urom
make
```

To create uROM files, please type on Windows:

```console
./urom.exe
```

Or on Linux:

```console
./urom
```

### ROM

To build the OS ROM image file, the map file with API handlers, and create documentation, please type:

```console
cd rom
make
```

### ASM

To build the application and create documentation, please type:

```console
cd asm
make
```

To compile ROM, please type on Windows:

```console
./asm.exe rom [input_file] [output_bin] [output_map]
```

Or on Linux:

```console
./asm rom [input_file] [output_bin] [output_map]
```

To compile an application, please type on Windows:

```console
./asm.exe app [input_file] [output_bin]
```

Or on Linux:

```console
./asm app [input_file] [output_bin]
```
