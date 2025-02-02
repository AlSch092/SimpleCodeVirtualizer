//Experimental code virtualization - AlSch092 @ Github
#include <iostream>
#include "VirtualMachine.hpp"

using namespace std;

#define DEFAULT_STACK_SIZE 1024

VirtualMachine* machine = new VirtualMachine(DEFAULT_STACK_SIZE); //make global instance instead of making each routine need a parameter for this type

/*
	Virtualized_AddIntegers - test for VM_PUSH, VM_ADD, VM_GET_TOP_STACK opcodes
*/
int Virtualized_AddIntegers()
{
    UINT a = 10, b = 15, c = 0;

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
        cout << "Virtualized_AddIntegers - bytecode executed successfully" << endl;
    }
    else
    {
        cout << "Failed to execute bytecode, please ensure bytecode is properly structured and doesn't reference non-existing registers" << endl;
    }

    return c;
}

float Virtualized_AddFloat()
{
    float a = 3.1415, b = 1.10, c = 0;

#ifdef USING_OBFUSCATE
    UINT bytecode[]
    {
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, (UINT)0 ,  //only opcodes should be obfuscated right now
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, (UINT)0 ,
        (UINT)VM_Opcode::VM_FL_ADD OBFUSCATE,
        (UINT)VM_Opcode::VM_GET_TOP_STACK OBFUSCATE, (UINT)&c,
        (UINT)VM_Opcode::VM_END_FUNC OBFUSCATE
    };
#else
    UINT bytecode[] 
    {
        (UINT)VM_Opcode::VM_PUSH, a,
        (UINT)VM_Opcode::VM_PUSH, b,
        (UINT)VM_Opcode::VM_ADD,
        (UINT)VM_Opcode::VM_GET_TOP_STACK, (UINT)&c,
        (UINT)VM_Opcode::VM_END_FUNC
    };
#endif

	memcpy((void*)&bytecode[1], &a, sizeof(UINT)); //obviously using memcpy is not ideal for scalability, hopefully we'll figure out some elegant way to mix types in bytecode - possibly union types?
    memcpy((void*)&bytecode[3], &b, sizeof(UINT));

    if (machine->Execute(bytecode, sizeof(bytecode) / sizeof(UINT)))
    {
        cout << "Virtualized_AddDouble - bytecode executed successfully" << endl;
    }
    else
    {
        cout << "Failed to execute bytecode, please ensure bytecode is properly structured and doesn't reference non-existing registers" << endl;
    }

    return c;
}

/*
	Virtualized_StdOut - test for VM_STDOUT opcode
*/
void Virtualized_StdOut()
{
    const char* text = "Hello from called routine";

#ifdef USING_OBFUSCATE
    UINT bytecode[]
    {
        (UINT)VM_Opcode::VM_STDOUT OBFUSCATE, (UINT)text,
        (UINT)VM_Opcode::VM_END_FUNC OBFUSCATE
    };
#else
    UINT bytecode[]
    {
        (UINT)VM_Opcode::VM_STDOUT, (UINT)&text,
        (UINT)VM_Opcode::VM_END_FUNC
    };
#endif

    if (machine->Execute(bytecode, sizeof(bytecode) / sizeof(UINT)))
    {
        cout << "Virtualized_CalledRoutine - bytecode executed successfully" << endl;
    }
    else
    {
        cout << "Failed to execute bytecode, please ensure bytecode is properly structured and doesn't reference non-existing registers" << endl;
    }
}

/*
    Virtualized_CalledRoutine - routine to be called by Virtualized_CallRoutine to prove that VM_CALL opcode works
*/
template<typename ...Args>
void Virtualized_CalledRoutine(Args ... vals) //routine called by Virtualized_CallRoutine
{
    ((cout << "Hello from called routine - parameter val=" << vals << endl), ...);
}

/*
	Virtualized_CallRoutine - test for VM_CALL opcode
*/
void Virtualized_CallRoutine()
{
    UINT a = 1000; //example parameters to be used in `Virtualized_CalledRoutine`
    UINT b = 2000;
    UINT c = 3000;
    UINT d = 4000;
    UINT e = 5000; //n-th parameter above 4 should go into rsp+20 + n*8
    UINT f = 6000;

    using CalledRoutineType = void(*)(UINT, UINT, UINT, UINT, UINT, UINT); //for the sake of testing different # of parameters, we'll use a function template /w parameter pack
    CalledRoutineType funcPtr_pack = &Virtualized_CalledRoutine<UINT, UINT, UINT, UINT, UINT, UINT>; //forward declare a function pointer since we're using parameter pack, otherwise we will get a compile error
    UINT callAddress = (UINT) funcPtr_pack;

#ifdef USING_OBFUSCATE
    UINT bytecode[] //while less space efficient, using default int size for each element instead of uint8_t allow us to pass local variables into the bytecode directly
    {
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, a,
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, b,
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, c,
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, d,
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, e,
        (UINT)VM_Opcode::VM_PUSH OBFUSCATE, f,
        (UINT)VM_Opcode::VM_CALL OBFUSCATE, 6, callAddress, //6 parameters
        (UINT)VM_Opcode::VM_END_FUNC OBFUSCATE
    };
#else
    UINT bytecode[]
    {
        (UINT)VM_Opcode::VM_PUSH, a,
        (UINT)VM_Opcode::VM_CALL, 1, callAddress,
        (UINT)VM_Opcode::VM_END_FUNC
    };
#endif

    if (machine->Execute(bytecode, sizeof(bytecode) / sizeof(UINT)))
    {
        cout << "Virtualized_CallRoutine - bytecode executed successfully" << endl;
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

    float result_f = Virtualized_AddFloat();

    cout << "result_f=" << result_f << " after adding `a` to `b`" << endl;

    Virtualized_CallRoutine();

    Virtualized_StdOut();

    return 0;
}
