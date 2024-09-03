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
CFLAGS		+=	-std=c23 -MMD -MP -Wall -Wextra -O2 -march=native
DEPS		:=	$(wildcard *.d)
LDFLAGS		+=
LDLIBS		+=


# Source code dependencies
LIBS			:= 	libffge.a libffge.so
LIBS_OBJS	 	:=	ffge.o			\
				ffge_prim_i8.o 		\
				ffge_prim_i8_helpers.o
ffge_prim_i8.o:			ffge.h

PROGS			:=	benchmark
$(PROGS):			$(LIBS_OBJS)

benchmark:			bench.o 		\
				utils.o			\
				xoshiro256ss.o

TESTS			:=	t-ffge			\
				t-ffge_prim		\
				t-ffge_prim_i8

$(TESTS):			$(LIBS_OBJS)		\
				utils.o			\
				xoshiro256ss.o


.PHONY:	all check clean debug distclean
.DEFAULT_GOAL := all

ifneq ($(DEPS),)
include $(DEPS)
endif

all: $(LIBS) $(PROGS) $(TESTS)

debug: $(PROGS) $(TESTS)
debug: ASFLAGS	+= -DDEBUG -Og -Fdwarf
debug: CFLAGS	+= -DDEBUG -g -Og

libffge.a: $(LIBS_OBJS)
	$(AR) rsc $@ $^

libffge.so: $(LIBS_OBJS)
	$(CC) -shared $(LDFLAGS) -o $@ $^ $(LDLIBS)

check: $(TESTS)
	@for tt in $(TESTS); do						\
		./$$tt && echo "$$tt: OK" ||				\
			( echo "$$tt: FAIL" ; exit 1)			\
	done

clean:
	$(RM) *.o *.d

distclean: clean
	$(RM) $(LIBS)
	$(RM) $(PROGS)
	$(RM) $(TESTS)

