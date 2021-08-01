; nasmfunc.asm
; TAB=4

;下面是实际的函数
[SECTION .text]	//目标文件中写了这些之后再写程序
	GLOBAL io_hlt	;程序中包含的函数名

io_hlt:	;表示void io_hlt(void);
	HLT
	RET	;相当于return


