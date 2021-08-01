
void io_hlt(void);

void HariMain(void)
{

fin:
    io_hlt();	//执行汇编文件里面的_io_hlt

    goto fin;
}
