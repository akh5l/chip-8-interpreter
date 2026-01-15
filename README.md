# Chip-8 Virtual Console Interpreter in C

A feature-complete implementation of the Chip-8 virtual console written in C, using SDL3 for display, input and sound.

## Download and build:
```bash
git clone https://github.com/akh5l/chip-8-interpreter.git
mkdir build && cd build
cmake .. && make
```

## Usage:
`./chip8`

ROMs cannot be specified as a parameter to the executable (yet!) so must be specified in src/main.c
