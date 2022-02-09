CC=gcc
CFLAGS=-g -lm
ASM=nasm
ASMFLAGS=-felf64

all: main-slow main-sse
clean:
	rm *.o
	rm main-slow
	rm main-sse

pixel.o: pixel.c pixel.h
	$(CC) $(CFLAGS) -c pixel.c

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c

bmp.o: bmp.c bmp.h image.h
	$(CC) $(CFLAGS) -c bmp.c

image-sepia-shared.o: image-sepia.h image.h pixel.h image-sepia-shared.c
	$(CC) $(CFLAGS) -c image-sepia-shared.c

image-sepia-slow-impl.o: image-sepia.h image.h pixel.h image-sepia-slow-impl.c
	$(CC) $(CFLAGS) -c image-sepia-slow-impl.c

image-sepia-fast-impl.o: image-sepia.h image-sepia-fast-impl.c image-sepia-fast-impl-helper.asm
	$(ASM) $(ASMFLAGS) image-sepia-fast-impl-helper.asm -o image-sepia-fast-impl-helper.o
	$(CC) $(CFLAGS) -c image-sepia-fast-impl.c -o image-sepia-fast-impl.c.o
	ld -r image-sepia-fast-impl.c.o image-sepia-fast-impl-helper.o -o image-sepia-fast-impl.o

main-slow: main.c bmp.o image.o image-sepia-shared.o image-sepia-slow-impl.o 
	$(CC) $(CFLAGS) -DSEPIA_SLOW_IMPL main.c bmp.o image.o image-sepia-shared.o image-sepia-slow-impl.o -o main-slow

main-sse: main.c bmp.c image.o image-sepia-shared.o image-sepia-fast-impl.o
	$(CC) $(CFLAGS) -DSEPIA_FAST_IMPL main.c bmp.o image.o image-sepia-shared.o image-sepia-fast-impl.o -o main-sse

