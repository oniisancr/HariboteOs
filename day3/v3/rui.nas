;   rui-os
;   TAB=4
;   有关BOOT_INFO
CYLS    EQU     0x0ff0          ;设定启动区
LEDS    EQU     0x0ff1          
VMODE   EQU     0x0ff2          ;关于颜色数目的信息。颜色的位数
SCRNX   EQU     0x0ff4          ;分辨率x
SCRNY   EQU     0x0ff6          ;分辨率y
VRAM    EQU     0x0ff8          ;图像缓冲区的开始地址

        ORG     0xc200          ;系统装载到内存的地址
        MOV     AL,0x13         ;设置显卡模式，VGA显卡
        MOV     AH,0x00         
        INT     0x10            ;调用显卡函数
        MOV     BYTE[VMODE],8   ;记录画面模式
        MOV     WORD[SCRNX],320
        MOV     WORD[SCRNY],400
        MOV     DWORD[VRAM],0x00a0000
fin:
        HLT
        JMP     fin