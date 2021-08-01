ipl.bin: ipl10.nas MakeFile
	nasm ipl10.nas -o ipl.bin -l ipl.lst
tail.bin: tail.nas MakeFile
	nasm tail.nas -o tail.bin -l tail.lst
rui.sys: rui.nas MakeFile
	nasm rui.nas -o rui.sys
helloos.img: ipl.bin tail.bin MakeFile
	cat ipl.bin tail.bin > helloos.img
load_sys:helloos.img rui.sys
	hdiutil mount helloos.img
	cp rui.sys /Volumes/HELLO-OS/rui.sys
	hdiutil unmount /Volumes/HELLO-OS
	make clean


asm:
	make -r ipl.bin
img:
	make -r helloos.img
clean:
	$(RM) *.bin
	$(RM) *.lst
img_only:
	make img
	make clean