#include "bootpack.h"

void init_gdtidt(void)
{
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000; //0x270000-027ffff为GDT,共64kb
    struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;
    int i;

    /*GDTの初期化*/
    // 8192個のGDTすべてのセグメントに対して，リミット0，ベース0，アクセス権属性0を付与
    for(i = 0; i < 8192; i++){
        set_segment(gdt + i, 0, 0, 0); //ポインタへの足し算は，実際は掛け算
    }
    set_segment(gdt + 1, 0xffffffff, 0x00000000, 0x4092); // セグメント番号1番に対する設定
    set_segment(gdt + 2, 0x0007ffff, 0x00280000, 0x409a); // セグメント番号2番に対する設定（bootpack.hrb用）
    load_gdtr(0xffff, 0x00270000);

    /* IDTの初期化 */
    for (i = 0; i <256; i++){
        set_gatedesc(idt+i, 0,0,0);
    }
    load_idtr(0x7ff, 0x0026f800);

    return;
}
void set_segment(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)	//设定上限、基址、访问权限=>0
{
    if (limit > 0xfffff){
        ar |= 0x8000; /* G_bit = 1*/		//‘|’表示或
        limit /= 0x1000;
    }
    sd->limit_low       = limit & 0xffff;	//‘&’表示与
    sd->base_low        = base & 0xffff;
    sd->base_mid        = (base >> 16) & 0xff;		//‘>>’表示右移
    sd->access_right    = ar & 0xff;
    sd->limit_high      = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high       = (base >> 24) & 0xff;
    return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)//ar :access right 访问权限
{
    gd->offset_low      = offset & 0xffff;
    gd->selector        = selector;
    gd->dw_count        = (ar >> 8) & 0xff;
    gd->access_right    = ar & 0xff;
    gd->offset_high     = (offset >> 16) & 0xffff;
    return;
}
