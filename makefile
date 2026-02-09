CC := gcc
CFLAGS := -g -fsanitize=address -Wall
LDFLAGS := -lm -lGL -lglfw -lpthread
LSAN := LSAN_OPTIONS=suppressions=lsan.supp

NOT_MINE_INCLUDE_DIRS := -I0_not_mine/include
NOT_MINE_SOURCES := $(shell find 0_not_mine -name '*.c')


ENGINE_EXECUTABLE := main
ENGINE_INCLUDE_DIRS := $(shell find engine -type d -exec echo -I{} \;)
ENGINE_SRCS := $(shell find engine -name '*.c' ! -name 'TEST.c')


TEST_ENGINE_EXECUTABLE := TEST
TEST_ENGINE_SRC := $(shell find engine -name '*.c' ! -name 'main.c')

main: $(ENGINE_SRCS)
	$(CC) $(CFLAGS) $(ENGINE_INCLUDE_DIRS) $(NOT_MINE_INCLUDE_DIRS) $(NOT_MINE_SOURCES) $(ENGINE_SRCS) $(LDFLAGS) -o builds/$(ENGINE_EXECUTABLE)

run: builds/main
	$(LSAN) builds/main

a: $(TEST_ENGINE_SRC)
	$(CC) $(CFLAGS) $(ENGINE_INCLUDE_DIRS) $(NOT_MINE_INCLUDE_DIRS) $(NOT_MINE_SOURCES) $(TEST_ENGINE_SRC) $(LDFLAGS) -o builds/$(TEST_ENGINE_EXECUTABLE)

clean:
	rm -rf builds/*

.PHONY: clean run