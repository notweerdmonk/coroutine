.DEFAULT_GOAL = all

ifneq ($(AVR),)
CC := avr-gcc -mmcu=atmega328p
OBJDUMP = avr-objdump -S -D $@ > $@.lst
else
CC := gcc
OBJDUMP :=
endif

OPTIMIZATION =

DEBUG_OPTIONS := -ggdb3 -save-temps=obj

CFLAGS = $(DEBUG_OPTIONS) $(OPTIMIZATION) -Wall -Wextra -Wno-implicit-fallthrough

ifneq ($(AVR),)
CFLAGS += -Os
endif

ifeq ($(INCLUDE),)
INCLUDE_DIRS := ../.
INCLUDE = $(foreach i, $(INCLUDE_DIRS), -I$i)
INCLUDE_HEADERS = $(foreach dir, $(abspath $(INCLUDE_DIRS)), $(wildcard $(dir)/*.h))
endif

LDFLAGS = 

LDLIBS = 

COMPILE = $(CC) $(CFLAGS) $(INCLUDE) -c -o"$@" "$<" $(LDLIBS) $(LDFLAGS)

SRCS :=\
coroutine_delegation.c \
coroutine_rpn.c

OBJS = $(SRCS:.c=.o)

STACK_USAGE_REPORTS = $(SRCS:.c=.su)

ASM_LISTINGS = $(SRCS:.c=.s)

PREPROCESSOR_OUTPUTS = $(SRCS:.c=.i)

OUTPUT_FILES = $(SRCS:.c=)

DISASM_LISTINGS = $(SRCS:.c=.lst)

all: $(OUTPUT_FILES)

$(OUTPUT_FILES): ./%: ./%.o
	$(CC) -o $@ $<
	$(OBJDUMP)

./%.o: ./%.c $(INCLUDE_HEADERS)
	$(COMPILE)

cleantemps:
	-$(RM) $(ASM_LISTINGS) $(PREPROCESSOR_OUTPUTS) \
		$(STACK_USAGE_REPORTS) \
		$(OBJS) $(OUTPUT_FILES) $(DISASM_LISTINGS)
	
clean: cleantemps
