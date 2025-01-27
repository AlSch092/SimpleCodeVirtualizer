.data


.code

VM_Jmp PROC ;  VM_Jmp(UINT callAddress)

    jmp rcx

VM_Jmp ENDP


; rcx = call addr
; rdx = num parameters
; r8 = parameter 1
; r9 = parameter 2
; TODO - find some clever way to handle any number of parameters, possibly need to put all function parameters into an array then pass the array to this routine and loop using num_parameters
VM_Call PROC

    push rax
    mov rax, rcx ; move callAddress into rax, shift all parameters into correct registers
    mov rcx, r8
    mov rdx, r9

    call rax
    pop rax
    ret

VM_Call ENDP

END