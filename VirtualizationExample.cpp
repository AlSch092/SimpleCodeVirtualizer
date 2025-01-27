//Experimental code virtualization - AlSch092 @ Github
#include <iostream>
#include "VirtualMachine.hpp"

using namespace std;

VirtualMachine* machine = new VirtualMachine(); //make global instance instead of making each routine need a parameter for this type

int Virtualized_AddIntegers()
{
    UINT a = 10, b = 15;
    UINT c = 0;

#ifdef USING_OBFUSCATE
    UINT bytecode[] //while less space efficient, using default int size for each element instead of uint8_t allow us to pass local variables into the bytecode directly
    {
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, a ,  //only opcodes should be obfuscated right now
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, b ,
        (UINT)VM_Opcode::VM_ADD OBFUSCATE,
        (UINT)VM_Opcode::VM_GET_TOP_STACK OBFUSCATE, (UINT)&c,
        (UINT)VM_Opcode::VM_END_FUNC OBFUSCATE
    };
#else
    UINT bytecode[] //while less space efficient, using default int size for each element instead of uint8_t allow us to pass local variables into the bytecode directly
    {
        (UINT)VM_Opcode::VM_PUSH, a,
        (UINT)VM_Opcode::VM_PUSH, b,
        (UINT)VM_Opcode::VM_ADD,
        (UINT)VM_Opcode::VM_GET_TOP_STACK, (UINT)& c,
        (UINT)VM_Opcode::VM_END_FUNC
    };
#endif

    if (machine->Execute(bytecode, sizeof(bytecode) / sizeof(UINT)))
    {
        cout << "bytecode executed successfully" << endl;
    }
    else
    {
        cout << "Failed to execute bytecode, please ensure bytecode is properly structured and doesn't reference non-existing registers" << endl;
    }

    return c;
}

void Virtualized_CalledRoutine(int val) //routine called by Virtualized_CallRoutine
{
	cout << "Hello from called routine - parameter val=" << val << endl;
}

void Virtualized_CallRoutine()
{
    UINT a = 1000;
    UINT callAddress = (UINT) & Virtualized_CalledRoutine;

#ifdef USING_OBFUSCATE
    UINT bytecode[] //while less space efficient, using default int size for each element instead of uint8_t allow us to pass local variables into the bytecode directly
    {
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, a,
        (UINT)VM_Opcode::VM_CALL OBFUSCATE, 1, callAddress,
        (UINT)VM_Opcode::VM_END_FUNC OBFUSCATE
    };
#else
    UINT bytecode[] //while less space efficient, using default int size for each element instead of uint8_t allow us to pass local variables into the bytecode directly
    {
        (UINT)VM_Opcode::VM_PUSH, a,
        (UINT)VM_Opcode::VM_CALL, 1, callAddress,
        (UINT)VM_Opcode::VM_END_FUNC
    };
#endif

    if (machine->Execute(bytecode, sizeof(bytecode) / sizeof(UINT)))
    {
        cout << "bytecode executed successfully" << endl;
    }
    else
    {
        cout << "Failed to execute bytecode, please ensure bytecode is properly structured and doesn't reference non-existing registers" << endl;
    }
}

int main()
{
    int result = Virtualized_AddIntegers();

    cout << "result=" << result << " after adding `a` to `b`" << endl;

    Virtualized_CallRoutine();

    return 0;
}
