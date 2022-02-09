ASM=nasm
ASM_FLAGS= -felf64
LINKER=ld

all: main

clean:
	rm *.o
	rm main

lib.o: lib.asm
	$(ASM) $(ASM_FLAGS) lib.asm -o lib.o

dict.o: dict.asm colon.inc
	$(ASM) $(ASM_FLAGS) dict.asm -o dict.o

main.o: main.asm words.inc colon.inc
	$(ASM) $(ASM_FLAGS) main.asm -o main.o

main: main.o lib.o dict.o
	$(LINKER) main.o lib.o dict.o -o main
