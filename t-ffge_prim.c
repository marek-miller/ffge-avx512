/* -------------------------------------------------------------------------- *
 * t-ffge_prim.c: Test the implementation of ffge_prim.                       *
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

#define REPS (33L)
#define MAX_SIZE (30)
static int64_t m[MAX_SIZE * MAX_SIZE];

#define SEED UINT64_C(342811)
static struct xoshiro256ss RNG;

/*
 * Compute a % FFGE_PRIM using only 64-bit wide registers and only addition
 * and bit/logical operations.  This function can be then vectorized easily.
 */
static int64_t modprim(int64_t a)
{
	int8_t s = a >= 0 ? 1 : -1;
	uint64_t b = (uint64_t)(a * s);

	uint64_t r = b & FFGE_PRIM;
	r += (b >> 31) & FFGE_PRIM;
	r += (b >> 62) & FFGE_PRIM;	/* b >= 0, so b>>62 is either 0 or 1 */

	/* at this point, r <= 2*FFGE_PRIM + 1 = 2^32 - 1*/
	r += (r >> 31);
	r &= FFGE_PRIM;
	if (r == FFGE_PRIM)
		r = 0;

	return (int64_t)(r * s);
}

static void test_modprim(void)
{
	for (size_t i = 0; i < 999999L; i++) {
		int64_t a = (int64_t)(xoshiro256ss_next(&RNG));
		TEST_ASSERT(modprim(a) == a % FFGE_PRIM,
			"i=%zu, a=%ld", i, a);
	}
}

static void test_ffge_prim_unit(void)
{
	int64_t m0[1] = { 0 };
	TEST_EQ(ffge_prim(m0, 1), 0);

	int64_t m1[1] = { 1 };
	TEST_EQ(ffge_prim(m1, 1), 1);
}

static void test_ffge_prim_two(void)
{
	int64_t m0[4] = { 0, 0, 0, 0 };
	TEST_EQ(ffge_prim(m0, 2), 0);

	int64_t m1[4] = { 0, 1, 0, 0 };
	TEST_EQ(ffge_prim(m1, 2), 1);

	int64_t m2[4] = { 0, 1, 1, 0 };
	TEST_EQ(ffge_prim(m2, 2), 2);
}


static void test_ffge_prim_randrank(size_t n)
{
	size_t r;

	for (size_t rep = 0; rep < REPS; rep++)
		for (size_t rank = 0; rank <= n; rank++) {
			ffge_mat_genrand_prim(m, n, rank, 99, &RNG);
			TEST_ASSERT((r = ffge(m, n)) == rank,
				"rank=%zu, rank_exp=%zu, n=%zu, rep=%zu",
					r, rank, n, rep);
		}

}

static void test_ffge_prim(void)
{
	test_ffge_prim_unit();

	test_ffge_prim_two();

	test_ffge_prim_randrank(3);
	test_ffge_prim_randrank(5);
	test_ffge_prim_randrank(12);
	test_ffge_prim_randrank(25);
}

static void TEST_MAIN(void)
{
	xoshiro256ss_init(&RNG, SEED);

	test_modprim();
	test_ffge_prim();
}
