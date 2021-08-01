ipl.bin: ipl.nas MakeFile
	nasm ipl.nas -o ipl.bin -l ipl.lst
tail.bin: tail.nas MakeFile
	nasm tail.nas -o tail.bin -l tail.lst
helloos4.img: ipl.bin tail.bin MakeFile
	cat ipl.bin tail.bin > helloos4.img

asm:
	make -r ipl.bin
img:
	make -r helloos4.img
clean:
	$(RM) *.bin
	$(RM) *.lst
src_only:
	make clean
	$(RM) *.img