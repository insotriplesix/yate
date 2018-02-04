EXEC := yate
CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -g -pedantic -O2
LDLIBS := -lncurses

INCDIR := inc
SRCDIR := src

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := -I $(INCDIR)

all: $(EXEC)

$(EXEC):
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) $(LDLIBS) -o $(EXEC)

.PHONY: clean

clean:
	rm -f $(EXEC)
