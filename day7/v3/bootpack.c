#include "bootpack.h"
extern struct KEYBUF keybuf;

void HariMain(void)
{
	
	struct BOOTINFO *binfo;
	extern char hankaku[4096];
	char s[40];//用于输出字符串	
	char mcursor[256];//存放鼠标形状
	int mx, my,i,j;
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
		io_cli();	//忽略中断
		if (keybuf.next == 0){
		    io_stihlt();
		} else {
		    i = keybuf.data[0];
		    keybuf.next--;
		    for(j=0;j<keybuf.next;j++){
			keybuf.data[j]=keybuf.data[j+1]; //向前移动
		    }
		    io_sti();
		    mysprintf(s, "%x", i);
		    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
		    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		}
	}

}



