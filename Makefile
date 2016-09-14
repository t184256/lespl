CFLAGS+=-fPIC -rdynamic
LDFLAGS+=-ldl

CFLAGS+=-std=gnu11
CFLAGS+=-Wall -Wextra -Wformat=2 -Wshadow
CFLAGS+=-Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=-Wno-unused-parameter
CFLAGS+=-Og -g -DDEBUG
CC=gcc
CFLAGS+=-fsanitize=address -fsanitize=leak -fsanitize=undefined

STDLIBS=$(patsubst %.c,%.so,$(wildcard std/*.c))
.PHONY: $(patsubst std/lib%.c,-l%,$(wildcard std/*.c))
OBJS=$(filter-out liblespl.o, $(patsubst %.c,%.o,$(wildcard *.c)))

all: main liblespl.so ${STDLIBS}

liblespl.o: ${OBJS}

std/libbool.so: std/libskip.so | -lskip
std/libint.so: std/libbool.so | -lbool
std/libstr.so: std/libint.so | -lint
std/libio.so: std/libint.so std/libstr.so std/libbool.so | -lbool -lint -lstr
std/libcontext.so: std/libstr.so | -lstr
std/libflow.so: std/libbool.so | -lbool
std/librestart.so: std/libbool.so | -lbool

# Temporary!
std/libflow.so: std/libint.so | -lint

%.o: %.c
	@echo "Compiling $@"
	@$(COMPILE.c) $(OUTPUT_OPTION) $< -MMD -MF $(patsubst %.c,%.mmd,$<)
-include $(wildcard *.mmd)
-include $(wildcard std/*.mmd)

liblespl.so: liblespl.c ${OBJS}
	@echo "Compiling $@"
	@$(CC) ${CFLAGS} -shared -o $@ $< ${OBJS} -MMD -MF liblespl.so.mmd
-include liblespl.so.mmd

%.so: %.c
	@echo "Compiling $@"
	@$(CC) ${CFLAGS} -shared -o $@ $< -MMD -MF $(patsubst %.c,%.so.mmd,$<) \
		-Lstd $|
-include $(wildcard *.so.mmd)
-include $(wildcard std/*.so.mmd)

main: main.c liblespl.so
	@echo "Linking   $@"
	@${CC} ${CFLAGS} -o $@ main.c ${LDFLAGS} -L. -llespl


.PHONY: run test clean
run: all
	LD_LIBRARY_PATH=.:std ./main

test: all
	@./test.sh

clean:
	@echo "Cleaning"
	@rm -f *.o *.so *.mmd std/*.o std/*.so std/*.mmd main

