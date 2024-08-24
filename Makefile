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
AS	:= nasm
ASFLAGS	+= -felf64 -w+all -w-reloc-rel-dword -Ox
CC	?= gcc
CFLAGS	+= -std=c23 -Wall -Wextra -O2 -march=native
LDFLAGS	+=
LDLIBS	+=

PROGS	:= benchmark
TESTS	:= t-ffge t-ffge_prim t-ffge_prim_i8


bench.o:		bench.h
ffge.o:			ffge.h
ffge_prim_i8.o:		ffge.h
utils.o:		utils.h
xoshiro256ss.o:		xoshiro256ss.h

benchmark:		bench.o 		\
			ffge.o			\
			ffge_prim_i8.o		\
			ffge_prim_i8_helpers.o	\
			utils.o			\
			xoshiro256ss.o

t-ffge.o:		test.h
t-ffge_prim.o:		test.h
t-ffge_prim_i8.o:	test.h

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

all: $(PROGS) $(BENCHES) $(TESTS)

debug: $(PROGS) $(BENCHES) $(TESTS)
debug: ASFLAGS	+= -DDEBUG -Og -Fdwarf
debug: CFLAGS	+= -DDEBUG -g -Og

check: $(TESTS)
	@for tt in $(TESTS); do						\
		./$$tt && echo "$$tt: OK" ||				\
			( echo "$$tt: FAIL" ; exit 1)			\
	done

clean:
	$(RM) *.o *.d
	$(RM) $(PROGS) $(BENCHES) $(TESTS)

