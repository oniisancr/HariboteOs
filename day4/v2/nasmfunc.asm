; nasmfunc.asm
; TAB=4

[section .text]
    GLOBAL  io_hlt, io_cli, io_sti, io_stihlt
    GLOBAL  io_in8, io_in16, io_in32
    GLOBAL  io_out8, io_out16, io_out32
    GLOBAL  io_load_eflags, io_store_eflags

io_hlt:        ; void io_hlt(void);
    HLT
    RET

io_cli:
    CLI		;忽略中断
    RET

io_sti:
    STI		;立即处理中断请求
    RET

io_stihlt:
    STI
    HLT
    RET

io_in8:
    MOV     EDX,[ESP+4]
    MOV     EAX,0
    IN      AL,DX
    RET

io_in16:
    MOV     EDX,[ESP+4]
    MOV     EAX,0
    IN      AX,DX
    RET

io_in32:
    MOV     EDX,[ESP+4]
    IN      EAX,DX
    RET

io_out8:
    MOV     EDX,[ESP+4]	;第一个参数
    MOV     AL,[ESP+8]	;第二个参数
    OUT     DX,AL	;将第二个参数写入到DX中
    RET

io_out16:
    MOV     EDX,[ESP+4]
    MOV     EAX,[ESP+8]
    OUT     DX,AX
    RET

io_out32:
    MOV     EDX,[ESP+4]
    MOV     AL,[ESP+8]
    OUT     DX,EAX
    RET

io_load_eflags:
    PUSHFD		;push flags double-word 将标志位压入栈
    POP     EAX		;EAX中的值就是返回值
    RET

io_store_eflags:
    MOV     EAX,[ESP+4]
    PUSH    EAX
    POPFD		;pop flags double-word	将标志位从栈中取出
    RET
