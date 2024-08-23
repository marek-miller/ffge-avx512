/* -------------------------------------------------------------------------- *
 * test.h: Test utilities.                                                    *
 *                                                                            *
 * Copyright 2024 Marek Miller & ⧉⧉⧉                                          *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify it    *
 * under the terms of the GNU General Public License as published by the      *
 * Free Software Foundation, either version 3 of the License, or (at your     *
 * option) any later version.                                                 *
 *                                                                            *
 * This program is distributed in the hope that it will be useful, but        *
 * WITHOUT ANY WARRANTY* without even the implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *
 * for more details.                                                          *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program.  If not, see <https://www.gnu.org/licenses/>.           *
 * -------------------------------------------------------------------------- */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static _Atomic bool TEST_RT = true;

#define TEST_FAIL(fmt, ...)	({					\
		fprintf(stderr, "%s:%d FAIL \"" fmt "\"\n",		\
			__FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__);	\
		TEST_RT = false;					\
	})

#define TEST_ASSERT(expr, fmt, ...)	({				\
		if (!(expr))						\
			TEST_FAIL(fmt __VA_OPT__(,) __VA_ARGS__);	\
	})

#define TEST_STR(a) #a
#define TEST_XSTR(a) TEST_STR(a)

#define TEST_EQ(a, b)	({						\
		TEST_ASSERT((a) == (b), "expected %s == %s",		\
			TEST_XSTR(a), TEST_XSTR(b));			\
	})


static void TEST_MAIN(void);

int main(int, char **)
{
	TEST_MAIN();

	return TEST_RT ? 0 : -1;
}

#endif /* TEST_H */
