; nasmfunc.asm
; TAB=4

[section .text]
    GLOBAL  io_hlt, io_cli, io_sti, io_stihlt
    GLOBAL  io_in8, io_in16, io_in32
    GLOBAL  io_out8, io_out16, io_out32
    GLOBAL  io_load_eflags, io_store_eflags
    GLOBAL  load_gdtr, load_idtr
    GLOBAL  asm_inthandler21, asm_inthandler27, asm_inthandler2c
    EXTERN  inthandler21, inthandler27, inthandler2c	;extern 声明函数在外部

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
load_gdtr:		; void load_gdtr(int limit, int addr);
    MOV	    AX,[ESP+4]		; limit
    MOV	    [ESP+6],AX
    LGDT    [ESP+6]
    RET

load_idtr:		; void load_idtr(int limit, int addr);
    MOV	    AX,[ESP+4]	; limit
    MOV	    [ESP+6],AX	; 注意操作
    LIDT    [ESP+6]
    RET
asm_inthandler21:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler21	;调用函数
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD		;中断处理完成后必须用IRETD返回

asm_inthandler27:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler27
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD

asm_inthandler2c:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler2c
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD
