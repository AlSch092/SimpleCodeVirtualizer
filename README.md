# SimpleCodeVirtualizer
A simple compile-time code virtualization class in C++. Still very early in development, soon we'll add in opcode randomization at runtime along with many more opcodes.

More information will be added here as the project is developed further. Anyone is welcome to contribute if you'd like to help implement opcodes.

## Features
- Modifiable number of general purpose registers, a stack, and instruction pointer
- Several working opcodes such as arithmetic, calling other routines, relative bytecode jumps, etc.
- Static obfuscation of VM opcodes which are deobfuscated JIT
- Randomization of VM opcodes which changes with each program instance (will be added soon)

## Limitations & Downsides
- Since Visual Studio in x64 does not allow inline asm, certain opcodes become cumbersome to implement, such as `VM_CALL` and `VM_JMP`
- Adding different calling conventions in x86 beyond __cdecl will need custom logic, along with certain function parameter aspects such as pass-by-reference, const, const reference, etc.
- Bytecode is large in size, each opcode & LHS/RHS values are 4/8 bytes each depending on whether you compile in 32-bit or 64-bit
