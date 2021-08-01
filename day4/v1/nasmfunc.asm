; nasmfunc.asm
; TAB=4

;下面是实际的函数
[section .text]	//目标文件中写了这些之后再写程序
	GLOBAL io_hlt	;程序中包含的函数名
	GlOBAL write_mem8;

io_hlt:	;表示void io_hlt(void);
	HLT
	RET	;相当于return

write_mem8:	;void write_mem8(int addr,int data)
	MOV	ECX,[ESP+4]	;第一个参数
	MOV	AL,[ESP+8]	;第二个参数
	MOV	[ECX],AL	;将第data放到addr位置上
	RET
