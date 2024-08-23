/* -------------------------------------------------------------------------- *
 * t-ffge.c: Test the implementation of ffge.                                 *
 *                                                                            *
 * Copyright 2024 ⧉⧉⧉                                                         *
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
#include "test.h"

#include <stddef.h>
#include <stdint.h>

#include "ffge.h"
#include "utils.h"
#include "xoshiro256ss.h"

#define REPS (3L)
#define MAX_SIZE (30)
static int64_t m[MAX_SIZE * MAX_SIZE];

#define SEED UINT64_C(1337488)
static struct xoshiro256ss RNG;

void test_ffge_unit(void)
{
	size_t rnk;

	int64_t m0[1] = { 0 };
	TEST_EQ(ffge(m0, 1, &rnk), 0);
	TEST_EQ(rnk, 0);

	int64_t m1[1] = { 1 };
	TEST_EQ(ffge(m1, 1, &rnk), 1);
	TEST_EQ(rnk, 1);
}

void test_ffge_two(void)
{
	size_t rnk;

	int64_t m0[4] = { 0, 0, 0, 0 };
	TEST_EQ(ffge(m0, 2, &rnk), 0);
	TEST_EQ(rnk, 0);

	int64_t m1[4] = { 0, 1, 0, 0 };
	TEST_EQ(ffge(m1, 2, &rnk), 0);
	TEST_EQ(rnk, 1);

	int64_t m2[4] = { 0, 1, 1, 0 };
	TEST_EQ(ffge(m2, 2, &rnk), 1);
	TEST_EQ(rnk, 2);
}


void test_ffge_randrank(size_t n)
{
	size_t r;

	for (size_t rep = 0; rep < REPS; rep++) {
		for (size_t rank = 0; rank <= n; rank++) {
			ffge_mat_genrand_prim(m, n, rank, 99, &RNG);
			TEST_EQ(ffge(m, n, &r), rank < n ? 0 : 1);
			TEST_ASSERT(r == rank,
				"n=%zu, rep=%zu, rank=%zu, r=%zu",
				n, rep, rank, r);
		}
	}
}

void test_ffge(void)
{
	test_ffge_unit();
	test_ffge_two();
	test_ffge_randrank(3);
	test_ffge_randrank(5);
	test_ffge_randrank(12);
	test_ffge_randrank(25);
}

void TEST_MAIN(void)
{
	xoshiro256ss_init(&RNG, SEED);

	test_ffge();
}
