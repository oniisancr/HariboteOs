#include "bootpack.h"

extern struct FIFO8 keyfifo,mousefifo;

#define PORT_KEYDAT             0x0060
#define PORT_KEYSTA             0x0064
#define PORT_KEYCMD             0x0064
#define KEYSTA_SEND_NOTREADY    0x02
#define KEYCMD_WRITE_MODE       0x60
#define KBC_MODE                0x47

// キーボードコントローラがデータ送信可能になるのを待つ
void wait_KBC_sendready(void)
{
    for (;;) {
        if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
            break;
        }
    }
    return;
}

// キーボードコントローラの初期化
void init_keyboard(void)
{
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}
struct MOUSE_DEC {
    unsigned char buf[3], phase;
    int x, y, btn;
};
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

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


#define KEYCMD_SENDTO_MOUSE     0xd4
#define MOUSECMD_ENABLE         0xf4

void enable_mouse(struct MOUSE_DEC *mdec)
{
    // マウス有効
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
    // うまく行くとACK(0xfa)が送信されてくる
    mdec->phase = 0; // マウスの0xfaを待っている段階
    return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
    if (mdec->phase == 0) {
        /* マウスの0xfaを待っている段階 */
        if (dat == 0xfa) {
            mdec->phase = 1;
        }
        return 0;
    }
    if (mdec->phase == 1) {
        /* マウスの1バイト目を待っている段階 */
        if ((dat & 0xc8) == 0x08) {
            /* 正しい1バイト目だった */
            mdec->buf[0] = dat;
            mdec->phase = 2;
        }
        return 0;
    }
    if (mdec->phase == 2) {
        /* マウスの2バイト目を待っている段階 */
        mdec->buf[1] = dat;
        mdec->phase = 3;
        return 0;
    }
    if (mdec->phase == 3) {
        /* マウスの3バイト目を待っている段階 */
        mdec->buf[2] = dat;
        mdec->phase = 1;
        mdec->btn = mdec->buf[0] & 0x07;
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];
        if ((mdec->buf[0] & 0x10) != 0) {
            mdec->x |= 0xffffff00;
        }
        if ((mdec->buf[0] & 0x20) != 0) {
            mdec->y |= 0xffffff00;
        }
        mdec->y = -(mdec->y); /* マウスではy方向の符号が画面と反対 */
        return 1;
    }
    return -1; /* ここに来ることはないはず */
}
