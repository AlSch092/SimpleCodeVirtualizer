# SimpleCodeVirtualizer
A simple compile-time code virtualization class in C++. Still very early in development, soon we'll add in opcode randomization at runtime along with many more opcodes.

More information will be added here as the project is developed further. Anyone is welcome to contribute if you'd like to help implement opcodes.

## Features
- Modifiable number of general purpose registers, a stack, and instruction pointer
- Opcodes include basic arithmetic, calling other routines, relative jumps; more will be added over time
- Static obfuscation of VM opcodes which are deobfuscated JIT
- Randomization of VM opcodes which changes with each program instance (will be added soon)

## Limitations & Downsides
- Since Visual Studio in x64 does not allow inline asm, certain opcodes become more difficult to implement, such as `VM_CALL` and `VM_JMP`
- Bytecode is large in size, each opcode & LHS/RHS values are 4/8 bytes each depending on whether you compile in 64 bit or 32
