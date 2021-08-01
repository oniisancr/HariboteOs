#include "bootpack.h"

void HariMain(void)
{
	
	struct BOOTINFO *binfo=(struct BOOTINFO *) ADR_BOOTINFO;
	char s[40];//用于输出字符串	
	char mcursor[256],keybuf[32],mousebuf[128];
    	struct MOUSE_DEC mdec;
	int mx, my,i;

    	init_gdtidt();
	init_pic();
    	io_sti();  //立即处理中断

        fifo8_init(&keyfifo, 32, keybuf);
    	fifo8_init(&mousefifo, 128, mousebuf);
	io_out8(PIC0_IMR, 0xf9); /* PIC1とキーボードを許可(11111001) --启用中断 */
	io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */


        init_keyboard();
	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

	mx = (binfo->scrnx - 16) / 2; /* 画面中央になるように座標計算 */
	my = (binfo->scrny - 28 - 16) /2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16 ,mx ,my ,mcursor, 16);
	mysprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	//内存检查
	i = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
	mysprintf(s, "memory %dMB", i);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

	enable_mouse(&mdec);

	for (;;) {
        io_cli();
        if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0){
            io_stihlt();
        } else {
            if (fifo8_status(&keyfifo) != 0) {
                i = fifo8_get(&keyfifo);
                io_sti();
                mysprintf(s, "%X", i);
                boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
                putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
            } else if (fifo8_status(&mousefifo) != 0)  {
                i = fifo8_get(&mousefifo);
                io_sti();
                if (mouse_decode(&mdec, i) != 0){
                    // データが3バイト揃ったので表示
                    mysprintf(s, "[lcr %d %d]", mdec.x, mdec.y);
                    // クリックしたボタンを大文字で表示
                    if ((mdec.btn & 0x01) != 0){
                        s[1] = 'L';
                    }
                    if ((mdec.btn & 0x02) != 0){
                        s[3] = 'R';
                    }
                    if ((mdec.btn & 0x04) != 0){
                        s[2] = 'C';
                    }
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
                    putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);

                    // マウスカーソルの移動
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15, my + 15); //マウス消す
                    // 移動量を加算
                    mx += mdec.x;
                    my += mdec.y;

                    // 左/上端にきたらx/y座標を維持
                    if (mx < 0 ){
                        mx = 0;
                    }
                    if (my < 0) {
                        my = 0;
                    }
                    // 右/下端（からマウスのサイズを引いた位置）にきたらx/y座標を維持
                    if (mx > binfo->scrnx - 16){
                        mx = binfo->scrnx - 16;
                    }
                    if (my > binfo->scrny - 16){
                        my = binfo->scrny - 16;
                    }
                    mysprintf(s, "(%d, %d)", mx, my);
                    boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15); // 座標消す
                    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s); // 座標書く
                    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16); // マウス書く
                }
            }
        }
    }
}
unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    // 386か、486なのかを確認
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT; // AC-bit = 1
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if ((eflg & EFLAGS_AC_BIT) != 0){ // 386ではAC=1しても自動で0に戻る
        flg486 = 1;
    }
    eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
    io_store_eflags(eflg);

    if (flg486 != 0){
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE; // キャッシュ禁止
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);

    if(flg486 != 0){
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE; // キャッシュ許可
        store_cr0(cr0);
    }

    return i;
}
