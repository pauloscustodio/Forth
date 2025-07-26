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

CXXFLAGS= -std=gnu++17 -MMD -Wall -Wextra -Werror -pedantic-errors -O3

SRCS	= $(wildcard *.cpp)
DEFS	= $(wildcard *.def)
OBJS 	= $(SRCS:.cpp=.o)
DEPENDS	= $(SRCS:.cpp=.d)

TEMPLATE= errors.cpp errors.h forth.cpp forth.h

ASTYLE	= astyle --style=attach --pad-oper --align-pointer=type \
		  --break-closing-braces --remove-braces --attach-return-type \
		  --max-code-length=80 --lineend=linux --formatted

TASKS	?= 9

all: $(PROJ)$(EXE)

$(PROJ)$(EXE): $(OBJS) $(DEFS) Makefile
	$(CXX) $(CXXFLAGS) -o $(PROJ)$(EXE) $(OBJS)
	perl -e update_readme.pl
	dos2unix README.md

# preprocess
$(OBJS): $(PROJ).pp

$(PROJ).pp: $(TEMPLATE) $(DEFS) mkcode.pl Makefile
	perl mkcode.pl $(TEMPLATE)
	touch $(PROJ).pp

clean:
	$(RM) $(PROJ) $(PROJ)$(EXE) $(OBJS) $(DEPENDS) $(wildcard *.o *.d *.i *.exe *.orig *.core *.bak *~)

test: $(PROJ)$(EXE)
	perl -S prove --state=slow,save -j$(TASKS) t/*.t

astyle:
	$(ASTYLE) *.c *.h

cloc:
	perl -S cloc *.cpp *.h *.def *.pl t/*.t t/*.pl

-include $(DEPENDS)
