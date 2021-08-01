; hello-os
; TAB=4

		ORG		0x7c00			; 指明程序的装载地址

; 以下的记述用于FAT12格式的软盘

		JMP		entry
		DB		0x90
		DB		"HELLOIPL"		; 可以自由的写引导扇区的名称
		DW		512				; 每个扇区的大小（必须是512）
		DB		1				; 簇的大小（必须是1个扇区）
		DW		1				; FAT从哪里开始（一般都是从第1扇区开始）
		DB		2				; FAT的个数（必须为2）
		DW		224				; 根目录的大小（通常设为224条目）
		DW		2880			; 磁盘大小（必须为2880扇区）
		DB		0xf0			; 磁盘种类（必须为0xf0)
		DW		9				; FAT的长度（必须为9扇区）
		DW		18				; 每个磁道有多少扇区（必须是18）
		DW		2				; 磁头数（必须是2）
		DD		0				; 不使用分区，必须是0
		DD		2880			; 磁盘大小再写一遍
		DB		0,0,0x29		; 固定
		DD		0xffffffff		; (可能是)卷码标号
		DB		"HELLO-OS   "	; 磁盘的名称（11字节）
		DB		"FAT12   "		; 磁盘格式的名称（8字节）
		RESB	18				; 先空出18字节
; 程序核心
entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
; 读磁盘
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
		MOV		AH,0x02			; AH=0x02 : ディスク読み込み
		MOV		AL,1			; 1个扇区
		MOV		BX,0
		MOV		DL,0x00			; Aドライブ
		INT		0x13			; ディスクBIOS呼び出し
		JC		error
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; 给SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用显卡BIOS
		JMP		putloop

fin:
		HLT						; 让CPU停止，等待指令
		JMP		fin				; 无限循环

error:
		MOV		SI,errormsg
		JMP		errorputloop
		
; 用来输出错误信息
errorputloop:
		MOV		AL,[SI]
		ADD		SI,1			; 给SI加1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用显卡BIOS
		JMP		errorputloop

errormsg:
		DB		0x0a, 0x0a		; 换行2次
		DB		"error: load disk"
		DB		0x0a			; 换行
		DB		0
msg:
		DB		0x0a, 0x0a		; 换行2次
		DB		"hello,rui"
		DB		0x0a			; 换行
		DB		0
		RESB 0x1fe-($-$$)		; 填定0x00，直到0x001fe
		DB		0x55, 0xaa
