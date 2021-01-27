OUTPUT:=shaibel
CFLAGS+=-Wall -Werror -Iinclude/ -ansi -pedantic
LIBS=
LDFLAGS+=
SRCS:=$(wildcard src/*.c)
OBJS:=$(patsubst src/%.c, objs/%.o, $(SRCS))
LDFLAGS+=$(patsubst %, -l%, $(LIBS))

.PHONY: default
default: $(OUTPUT)

objs/%.o: src/%.c
	@mkdir -p objs/
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTPUT): $(OBJS)
	$(CC) $^ -o $(OUTPUT) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf objs/ $(OUTPUT)

.PHONY: debug
debug: CFLAGS += -D__DEBUG -g -O0
debug: default

.PHONY: test
test: CFLAGS += -D__TEST -g -O0
test: debug

.PHONY: perf
perf: CFLAGS += -g -O3 -DNDEBUG
perf: clean default

.PHONY: perftest
perftest: CFLAGS += -D__TEST -g -DNDEBUG -DPERFTEST -O3
perftest: default
