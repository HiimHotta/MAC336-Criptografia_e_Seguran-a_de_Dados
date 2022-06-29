OUTPUT=main

CC=gcc
CC_OPT=-std=c11


.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OUTPUT).c hexdump.h
	$(CC) -o $(OUTPUT) $(CC_OPT) $(OUTPUT).c


.PHONY: clean
clean:
	rm $(OUTPUT)