# --------------------------------------------------------------------------- #
# Copyright 2024 Marek Miller & ⧉⧉⧉                                           #
#                                                                             #
# This program is free software: you can redistribute it and/or modify it     #
# under the terms of the GNU General Public License as published by the       #
# Free Software Foundation, either version 3 of the License, or (at your      #
# option) any later version.                                                  #
#                                                                             #
# This program is distributed in the hope that it will be useful, but         #
# WITHOUT ANY WARRANTY# without even the implied warranty of MERCHANTABILITY  #
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    #
# for more details.                                                           #
#                                                                             #
# You should have received a copy of the GNU General Public License along     #
# with this program.  If not, see <https://www.gnu.org/licenses/>.            #
# --------------------------------------------------------------------------- #
AS		:=	nasm
ASFLAGS		+=	-felf64 -w+all -w-reloc-rel-dword -Ox
CC		?=	gcc
CFLAGS		+=	-std=c23 -Wall -Wextra -O2 -march=native
LDFLAGS		+=
LDLIBS		+=


# Source code dependencies
LIBS		:= 	libffge.a
PROGS		:=	benchmark

FFGE_OBJS 	:=	ffge.o			\
			ffge_prim_i8.o 		\
			ffge_prim_i8_helpers.o
$(FFGE_OBJS):		ffge.h

bench.o:		bench.h
utils.o:		utils.h
xoshiro256ss.o:		xoshiro256ss.h

benchmark:		bench.o 		\
			$(FFGE_OBJS)		\
			utils.o			\
			xoshiro256ss.o

# Tests
TESTS		:=	t-ffge			\
			t-ffge_prim		\
			t-ffge_prim_i8

TEST_OBJS	:=	t-ffge.o		\
			t-ffge_prim.o		\
			t-ffge_prim_i8.o
$(TEST_OBJS):		test.h

t-ffge:			t-ffge.o		\
			ffge.o			\
			utils.o			\
			xoshiro256ss.o

t-ffge_prim:		t-ffge_prim.o		\
			ffge.o			\
			utils.o			\
			xoshiro256ss.o

t-ffge_prim_i8:		t-ffge_prim_i8.o	\
			ffge_prim_i8.o		\
			ffge_prim_i8_helpers.o	\
			utils.o			\
			xoshiro256ss.o


.PHONY:	all check clean debug
.DEFAULT_GOAL := all

all: $(LIBS) $(PROGS) $(TESTS)

debug: $(PROGS) $(TESTS)
debug: ASFLAGS	+= -DDEBUG -Og -Fdwarf
debug: CFLAGS	+= -DDEBUG -g -Og

libffge.a: $(FFGE_OBJS)
	$(AR) rsc $@ $^


check: $(TESTS)
	@for tt in $(TESTS); do						\
		./$$tt && echo "$$tt: OK" ||				\
			( echo "$$tt: FAIL" ; exit 1)			\
	done

clean:
	$(RM) *.o *.d
	$(RM) $(LIBS)
	$(RM) $(PROGS)
	$(RM) $(TESTS)

