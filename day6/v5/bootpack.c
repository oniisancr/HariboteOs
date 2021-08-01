#include "bootpack.h"

void HariMain(void)
{
	
	struct BOOTINFO *binfo;
	extern char hankaku[4096];
	char s[40];//用于输出字符串	
	char mcursor[256];//存放鼠标形状
	int mx, my;
    	init_gdtidt();
	init_pic();
    	io_sti();  //立即处理中断

	binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	putfonts8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "ABC 123");
	putfonts8_asc(binfo->vram, binfo->scrnx,31,31, COL8_000000, "Haribote OS.");
	putfonts8_asc(binfo->vram, binfo->scrnx,30,30, COL8_FFFFFF, "Haribote OS");
	
	mx = (binfo->scrnx - 16) / 2; /* 画面中央になるように座標計算 */
	my = (binfo->scrny - 28 - 16) /2;

	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16 ,mx ,my ,mcursor, 16);

	io_out8(PIC0_IMR, 0xf9); /* PIC1とキーボードを許可(11111001) --启用中断 */
	io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */
	for(;;){
		io_hlt();
	}

}


