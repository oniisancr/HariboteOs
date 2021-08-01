ipl.bin: ipl.nas MakeFile
	nasm ipl.nas -o ipl.bin -l ipl.lst
tail.bin: tail.nas MakeFile
	nasm tail.nas -o tail.bin -l tail.lst
helloos5.img: ipl.bin tail.bin MakeFile
	cat ipl.bin tail.bin > helloos5.img

asm:
	make -r ipl.bin
img:
	make -r helloos5.img
clean:
	$(RM) *.bin
	$(RM) *.lst
img_only:
	make img
	make clean