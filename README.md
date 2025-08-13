
Overview
This project is a custom two-pass assembler written in C for a simplified assembly language.
The assembler reads assembly source files (.as), translates them into machine code, and outputs the result into object and listing files.

Features
Two-pass processing:

First pass: builds symbol and data tables, detects syntax errors.

Second pass: generates final machine code.

Supports a predefined set of assembly instructions and addressing modes.

Outputs object files, entry files, and external files.

Error handling with descriptive messages.

Compatible with Ubuntu using gcc compilation.

Compilation
Run the following in the project directory:

go
Copy
Edit
make
Usage
After compilation, run:

csharp
Copy
Edit
./assembler file1.as file2.as ...
Each .as file will be assembled, and the corresponding output files will be generated in the same directory.

File Structure
main.c – Program entry point

parser.c, parser.h – Parsing logic

assembler.c, assembler.h – Assembler core

tables.c, tables.h – Symbol and data table management

utils.c, utils.h – Helper functions

makefile – Build configuration

Requirements
GCC compiler

Ubuntu OS
