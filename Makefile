.PHONY: build
build: lab4.2

lab4.2: main.c
	gcc --std=c11 -Wall -O0 -o $@ -g $^

.PHONY: asm
asm: main.c
	$(CC) $(CFLAGS) -S -o /dev/stdout $^ | less
