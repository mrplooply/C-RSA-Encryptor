CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic

.PHONY: all clean debug

all: encode decode

encode: encode.o code.o huffman.o io.o node.o pq.o stack.o
	$(CC) -o $@ $^

decode: decode.o code.o huffman.o io.o node.o pq.o stack.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

debug: CFLAGS += -gdwarf-4
debug: clean all

clean:
	rm -f  *.o

spotless: clean
	rm -f decode encode

format:
	clang-format -i -style=file *.c
