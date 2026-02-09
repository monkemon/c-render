CC := gcc
CFLAGS := -Inot_mine -Iinclude -Iutils -Iloaders -g -fsanitize=address -Wall
LDFLAGS := -lm -lGL -lglfw -lpthread
LSAN := LSAN_OPTIONS=suppressions=lsan.supp
EXECUTABLE := main


SOURCES := main.c not_mine/src/glad.c utils/*.c loaders/*.c
HEADERS := $(wildcard *.h include/*.h)

SOURCES_TEST := TEST.c utils/*.c loaders/*.c

main: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $(EXECUTABLE)


run: main
	$(LSAN) ./main

clean:
	rm ./main

a: TEST.c
	$(CC) $(CFLAGS) -lm -lpthread $(SOURCES_TEST) -o TEST && ./TEST

.PHONY: clean run