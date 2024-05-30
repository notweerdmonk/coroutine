CC := gcc

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

EXECUTABLES := statham duffs_device

CFLAGS := -ggdb3 -Wall -Wextra -Wno-implicit-fallthrough

all: $(EXECUTABLES)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECTS): $(SOURCES)

$(EXECUTABLES): ./%: ./%.o
	$(CC) -o $@ $<

clean:
	rm -f $(OBJECTS) $(EXECUTABLES)
