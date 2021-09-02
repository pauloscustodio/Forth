#------------------------------------------------------------------------------
# C implementation of a Forth interpreter
# Copyright (c) Paulo Custodio, 2020-2021
# License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
#------------------------------------------------------------------------------

ifeq ($(OS),Windows_NT)
  EXE 	= .exe
else
  EXE 	=
endif

CFLAGS	= -MMD -Wall -O3

SRCS 	= $(wildcard *.c)
DEFS	= $(wildcard *.def)
OBJS 	= $(SRCS:.c=.o)
DEPENDS	= $(SRCS:.c=.d)

ASTYLE	= astyle --style=attach --pad-oper --align-pointer=type \
		  --break-closing-braces --remove-braces --attach-return-type \
		  --max-code-length=80 --lineend=linux --formatted

all: forth$(EXE)

forth$(EXE): $(OBJS) $(DEFS)
	gcc $(CFLAGS) -o forth$(EXE) $(OBJS)

clean:
	$(RM) forth$(EXE) $(OBJS) $(DEPENDS) $(wildcard *.o *.d *.i *.exe *.orig *.core *.bak *~)

test: forth$(EXE)
	perl -S prove --state=slow,save -j9 t/*.t

astyle:
	$(ASTYLE) *.c *.h

-include $(DEPENDS)
