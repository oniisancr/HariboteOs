; hello-os
; TAB=4
CYLS	EQU		10				; どこまで読み込むか 定义常量
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
readloop:
		MOV		SI,0			;记录失败的次数
retry:
		MOV		AH,0x02			; AH=0x02 : ディスク読み込み
		MOV		AL,1			; 1个扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; ディスクBIOS呼び出し
		JNC		next			;没有出错就跳转到next
		ADD		SI,1			;往SI加1
		CMP		SI,5
		JAE		error			;SI>=5,跳转
		MOV		AH,0x00			
		MOV		DL,0x00			
		INT		0x13			;重置驱动器
		JMP		retry

next:
		MOV		AX,ES			; 后移0x200
		ADD		AX,0x0020		; ADD AX,512/16
		MOV		ES,AX			; ADD ES,0x020 ES是读入地址
		ADD		CL,1
		CMP		CL,18
		JBE		readloop		; CL <= 18 だったらreadloop
		MOV		CL,1			;扇区1
		ADD		DH,1			;改变磁头
		CMP		DH,2			
		JB		readloop		; DH < 2 だったらreadloop
		MOV		DH,0
		ADD		CH,1			;柱面加1
		CMP		CH,CYLS			;读入10个柱面
		JB		readloop		; CH < CYLS だったらreadloop
		JMP		welcome			;读完18个扇区后进入欢迎界面

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

welcome:
		MOV		SI,msg
		JMP		putloop

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
