CC=gcc
EXECUTE=bmp
CODE=tema3.c

build: $(EXECUTE)

$(EXECUTE): $(CODE)
	$(CC) $(CODE) bmp_header.h -o $(EXECUTE)

run:
	./$(EXECUTE)

.PHONY: clean

clean:
	rm -f *.o $(EXECUTE)