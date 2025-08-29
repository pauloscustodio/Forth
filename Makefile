#------------------------------------------------------------------------------
# C++ implementation of a Forth interpreter
# Copyright (c) Paulo Custodio, 2020-2025
# License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
#------------------------------------------------------------------------------

PROJ	= forth

ifeq ($(OS),Windows_NT)
  EXE 	= .exe
else
  EXE 	=
endif

CXXFLAGS= -std=gnu++17 -MMD -Wall -Wextra -Wpedantic -Werror -O3

SRCS 	= $(wildcard *.cpp)
DEFS	= $(wildcard *.def)
OBJS 	= $(SRCS:.cpp=.o)
DEPENDS	= $(SRCS:.cpp=.d)

ASTYLE	= astyle --style=attach --pad-oper --align-pointer=type \
		  --break-closing-braces --add-braces --attach-return-type \
		  --max-code-length=80 --lineend=linux --formatted

all: $(PROJ)$(EXE)

$(PROJ)$(EXE): $(OBJS) $(DEFS) Makefile $(wildcard *.pl)
	$(CXX) $(CXXFLAGS) -o $(PROJ)$(EXE) $(OBJS)
	perl update_words.pl
	dos2unix README.md

clean:
	$(RM) $(PROJ) $(PROJ)$(EXE) $(OBJS) $(DEPENDS) $(wildcard *.o *.d *.i *.exe *.orig *.core *.bak *~)

test: $(PROJ)$(EXE)
	perl -S prove --state=slow,save t/*.t

astyle:
	$(ASTYLE) *.cpp *.h

cloc:
	perl -S cloc *.cpp *.h *.def *.pl t/*.t t/*.pl

-include $(DEPENDS)
