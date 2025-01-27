//Experimental code virtualization - AlSch092 @ Github
#pragma once
#include <stdint.h>
#include <mutex>
#include <unordered_map> //for opcode mappings once we add in randomization
#include <random>

#define USING_OBFUSCATE //comment this out to disable opcode obfuscation

#ifdef USING_OBFUSCATE
#define XOR_KEY 0x1234567
#define OBFUSCATE   ^ XOR_KEY
#define DEOBFUSCATE OBFUSCATE
#endif

#ifdef _M_X64  
#define UINT uint64_t

extern "C"
{
    void VM_Call(UINT callAddress, UINT numParameters); //asm stub for VM_CALL opcode since we can't inline
}

#else
#define UINT uint32_t
#endif

enum class VM_Opcode : UINT //these can be randomized at runtime on each instance of the program , we'll implement this soon
{
    VM_PUSH,
    VM_POP,

    VM_ADD, //int
    VM_SUB,
    VM_MUL,
    VM_DIV,

    VM_FL_ADD, //float
    VM_FL_SUB,
    VM_FL_MUL,
    VM_FL_DIV,

    VM_MOVE, //move address to address
    VM_MOV_REGISTER_TO_REGISTER,
    VM_MOV_IMMEDIATE_TO_REGISTER,

    VM_GET_TOP_STACK,

    VM_CALL,
    VM_JMP_OFFSET, //modify IP
    VM_JMP_ABSOLUTE, //jump outside of bytecode? might not be feasible in VS x64 since we need to call an asm stub which jumps, which requires atleast one register modification and thus is not a perfect jmp

    VM_STDOUT,
    VM_DBG_BREAK,

    VM_NOP,
    VM_END_FUNC //each bytecode block must end with this opcode
};

class VirtualMachine
{
public:

    /*
        bool Execute(UINT* virtualizedCode, uint32_t executeSize) - executes bytecode
        returns `true` on success, `false` on failure
    */
    bool Execute(UINT* bytecode, uint32_t executeSize)
    {
        if (bytecode == nullptr || executeSize == 0)
            return false;

        ip = (UINT)&bytecode[0];

        //adding a RAII lock means __try/__except won't compile without errors - it's up to the caller to ensure we don't dereference unallocated memory or execute past the buffer
        std::lock_guard<std::mutex> lock(execution_mtx); //multi threading could potentially lead to sp/ip corruption, so use a mutex

        for (uint32_t i = 0; i < executeSize; i++)
        {
            VM_Opcode vm_opcode = *(VM_Opcode*)ip;
            ip += sizeof(UINT);

        #ifdef USING_OBFUSCATE
            vm_opcode = (VM_Opcode)((UINT)vm_opcode DEOBFUSCATE);
        #endif

            switch (vm_opcode)
            {
            case VM_Opcode::VM_PUSH: //push = write to stack, increment sp
                stack[sp++] = *(UINT*)ip;
                ip += sizeof(UINT);
                break;

            case VM_Opcode::VM_POP:
                sp--;
                break;

            case VM_Opcode::VM_ADD: //arithmetic operations (+,-,*,/) pop two values from the stack and place the result in the into stack's sp index
            {
                UINT b = stack[--sp];
                UINT a = stack[--sp];
                stack[sp] = a + b;
            } break;

            case VM_Opcode::VM_SUB:
            {
                UINT b = stack[--sp];
                UINT a = stack[--sp];
                stack[sp] = a - b;
            } break;

            case VM_Opcode::VM_MUL:
            {
                UINT b = stack[--sp];
                UINT a = stack[--sp];
                stack[sp] = a * b;
            } break;

            case VM_Opcode::VM_DIV:
            {
                UINT b = stack[--sp];
                UINT a = stack[--sp];
                stack[sp] = a / b;
            } break;

            case VM_Opcode::VM_MOV_REGISTER_TO_REGISTER: // ex. mov 0, 1   (move register 1 into register 0, similar to mov ax,bx)
            {
                UINT lhs_index = *(UINT*)ip;
                ip += sizeof(UINT);

                UINT rhs_index = *(UINT*)ip;
                ip += sizeof(UINT);

                if (lhs_index < MAX_REGISTERS && rhs_index < MAX_REGISTERS)
                    registers[lhs_index] = registers[rhs_index];
                else
                    return false; //invalid register index
            } break;

            case VM_Opcode::VM_MOV_IMMEDIATE_TO_REGISTER: //ex. mov ax, 12345678
            {
                UINT register_index = *(UINT*)ip; //should be 0 through MAX_REGISTERS-1 (0-indexed)
                ip += sizeof(UINT);

                UINT value = *(UINT*)ip;
                ip += sizeof(UINT);

                if (register_index < MAX_REGISTERS)
                    registers[register_index] = value;
                else
                    return false; //invalid register index
            } break;

            case VM_Opcode::VM_GET_TOP_STACK: // mov myVar, [sp]
            {
                UINT varAddress = *(UINT*)ip;
                ip += sizeof(UINT);
                *(UINT*)varAddress = stack[sp];
            }break;

            case VM_Opcode::VM_JMP_OFFSET:
            {
				int offset = *(int*)ip;
                ip += (sizeof(UINT) + offset);
            }break;

            case VM_Opcode::VM_CALL: //x86 works okay, however in x64, functions with parameters are not supported yet, this will be added shortly
            {
                UINT numParameters = *(UINT*)ip;
				ip += sizeof(UINT);
                UINT callAddress = *(UINT*)ip;

#ifdef _M_X64  
                VM_Call(callAddress, numParameters); //parameters need to correctly go into rcx, rdx, r8, r9, [rsp+...]
#else
                for (int i = 0; i < numParameters; i++) //x86 calling convention, push parameters onto stack then call
                {
					UINT parameter = stack[sp - numParameters + i];

                    __asm { push parameter }
                }

                __asm
                {
                    call callAddress
                }

                for (int i = 0; i < numParameters; i++)
                {
                    __asm { add esp, 4 }
                }
#endif
            }break;

            case VM_Opcode::VM_NOP: //do nothing
                break;

            case VM_Opcode::VM_END_FUNC: //since many opcodes increment IP, our executeSize won't map directly to the number of UINT's in the bytecode
                i = executeSize;
                break;

            default: //opcode unknown
                break;
            };
        }

        ip = 0;

		for (int i = 0; i < MAX_REGISTERS; i++)
			registers[i] = 0;

        return true;
    }

private:
    const static int MAX_REGISTERS = 8; //can be increased if needed

    UINT registers[MAX_REGISTERS]{ 0 };  //general purpose

    UINT ip = 0;
    UINT sp = 0;

    UINT stack[1024]{ 0 };

    std::mutex execution_mtx;

    std::unordered_map<UINT, UINT> opcodeMappings; //for randomizing opcodes
};
