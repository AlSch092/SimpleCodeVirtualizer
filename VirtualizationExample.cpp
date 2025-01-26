//Experimental code virtualization - AlSch092 @ Github
#include <iostream>
#include <stdint.h>
#include "VirtualMachine.hpp"

using namespace std;

VirtualMachine* machine = new VirtualMachine(); //make global instance instead of making each routine need a parameter for this type

int ExampleVirtualizedFunction()
{
    UINT a = 10, b = 15;
    UINT c = 0;

    UINT bytecode[] //while less space efficient, using default int size for each element instead of uint8_t allow us to pass local variables into the bytecode directly
    {
        (UINT)VM_Opcode::VM_PUSH, a,
        (UINT)VM_Opcode::VM_PUSH, b,
        (UINT)VM_Opcode::VM_ADD,
        (UINT)VM_Opcode::VM_GET_TOP_STACK, (UINT)& c,
        (UINT)VM_Opcode::VM_END_FUNC
    };

    machine->Execute(bytecode, sizeof(bytecode) / sizeof(UINT));

    return c;
}

int main()
{
    int result = ExampleVirtualizedFunction();

    cout << "result=" << result << " after adding `a` to `b`" << endl;

    return 0;
}
