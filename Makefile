#Makefile for Preisach Model Main code

all: src/_PreisachCode.so src/_PMuserinput.so src/_camCalibrate.so

#####LINK OBJECTS INTO FINAL OUTPUT FILE#####
src/_PreisachCode.so: obj/PreisachCode.o obj/PreisachCode_wrap.o obj/framebuffer.o obj/internals.o
	ld -shared obj/PreisachCode.o obj/PreisachCode_wrap.o  obj/framebuffer.o obj/internals.o -o src/_PreisachCode.so

src/_PMuserinput.so: obj/PreisachCode2.o obj/PreisachCode2_wrap.o obj/framebuffer.o obj/internals.o
	ld -shared obj/PreisachCode2.o obj/PreisachCode2_wrap.o obj/framebuffer.o obj/internals.o -o src/_PMuserinput.so

src/_camCalibrate.so: obj/camCalibrate.o obj/camCalibrate_wrap.o
	ld -shared obj/camCalibrate.o obj/camCalibrate_wrap.o -o src/_camCalibrate.so
#####COMPILE OBJECTS#####
obj/camCalibrate.o: src/camCalibrate.c
	gcc -c src/camCalibrate.c -Iinclude -o obj/camCalibrate.o

obj/camCalibrate_wrap.o: src/camCalibrate_wrap.c
	gcc -c src/camCalibrate_wrap.c -I/usr/include/python2.7 -o obj/camCalibrate_wrap.o

obj/PreisachCode.o: src/PreisachCode.c
	gcc -c src/PreisachCode.c -Iinclude/ -o obj/PreisachCode.o

obj/PreisachCode_wrap.o: src/PreisachCode_wrap.c
	gcc -c src/PreisachCode_wrap.c -Iinclude -I/usr/include/python2.7 -o obj/PreisachCode_wrap.o

obj/PreisachCode2.o: src/PreisachCode2.c
	gcc -c src/PreisachCode2.c -Iinclude -o obj/PreisachCode2.o

obj/PreisachCode2_wrap.o: src/PreisachCode2_wrap.c
	gcc -c src/PreisachCode2_wrap.c -Iinclude -I/usr/include/python2.7 -o obj/PreisachCode2_wrap.o

obj/framebuffer.o: src/framebuffer.c
	gcc -c src/framebuffer.c -Iinclude -o obj/framebuffer.o

obj/internals.o: src/internals.c
	gcc -c src/internals.c -Iinclude -o obj/internals.o

#####SWIG WRAPPING#####
src/PreisachCode2_wrap.c: swig_headers/PMuserinput.i
	swig -python -Iinclude -o src/PreisachCode2_wrap.c swig_headers/PMuserinput.i

src/PreisachCode_wrap.c: swig_headers/PreisachCode.i
	swig -python -Iinclude -o src/PreisachCode_wrap.c swig_headers/PreisachCode.i

src/camCalibrate_wrap.c: swig_headers/camCalibrate.i
	swig -python -Iinclude -o src/camCalibrate_wrap.c swig_headers/camCalibrate.i
