//Experimental code virtualization - AlSch092 @ Github
#pragma once

#ifdef _M_X64  
#define UINT uint64_t
#else
#define UINT uint32_t
#endif

enum  class VM_Opcode : UINT //these can be randomized at runtime on each instance of the program , we'll implement this soon
{
    VM_PUSH,
    VM_POP,
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    VM_MOVE,
    VM_MOV_REGISTER_TO_REGISTER,
    VM_MOV_IMMEDIATE_TO_REGISTER,
    VM_GET_TOP_STACK,
    VM_HALT,
    VM_END_FUNC
};

class VirtualMachine
{
public:

    void Execute(UINT* virtualizedCode, uint32_t executeSize)
    {
        if (virtualizedCode == nullptr || executeSize == 0)
            return;

        ip = (UINT)&virtualizedCode[0];

        for (uint32_t i = 0; i < executeSize; i++)
        {
            VM_Opcode vm_opcode = *(VM_Opcode*)ip;
            ip += sizeof(UINT);

            switch (vm_opcode)
            {
            case VM_Opcode::VM_PUSH: //push = write to stack, increment sp
                stack[sp++] = *(UINT*)ip;
                ip += sizeof(UINT);
                break;

            case VM_Opcode::VM_POP:
                sp--;
                break;

            case VM_Opcode::VM_ADD:
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

            case VM_Opcode::VM_MOV_REGISTER_TO_REGISTER: // mov ax, bx
            {
                UINT lhs_index = *(UINT*)ip;
                ip += sizeof(UINT);

                UINT rhs_index = *(UINT*)ip;
                ip += sizeof(UINT);

                registers[lhs_index] = registers[rhs_index];
            } break;

            case VM_Opcode::VM_MOV_IMMEDIATE_TO_REGISTER: //mov ax, 12345678
            {
                UINT lhs_index = *(UINT*)ip;
                ip += sizeof(UINT);

                UINT value = *(UINT*)ip;
                ip += sizeof(UINT);

                registers[lhs_index] = value; //careful, if lhs_index is above 7 , we get a buffer overrun

            } break;

            case VM_Opcode::VM_GET_TOP_STACK: // mov myVar, [sp]
            {
                UINT varAddress = *(UINT*)ip;
                ip += sizeof(UINT);
                *(UINT*)varAddress = stack[sp];
            }break;

            case VM_Opcode::VM_HALT: //do nothing
                break;

            case VM_Opcode::VM_END_FUNC: //since many opcodes increment IP, our executeSize won't map directly to the number of UINT's in the bytecode
                i = executeSize;
                break;

            default: //opcode unknown
                break;
            };
        }
    }

private:
    const static int MAX_REGISTERS = 8; //can be increased if needed

    UINT registers[MAX_REGISTERS]{ 0 };  //general purpose

    UINT ip = 0;
    UINT sp = 0;

    UINT stack[1024]{ 0 };
};
