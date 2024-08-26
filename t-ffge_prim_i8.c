/* -------------------------------------------------------------------------- *
 * t-ffge_prim_i8.c: Test the implementation of ffge_prim_i8                  *
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

#define REPS (999L)

#define SEED UINT64_C(30393)
static struct xoshiro256ss RNG;

#define MAX_SIZE (28)
static int64_t m[MAX_SIZE * MAX_SIZE];
static alignas(64) int64_t m_i8[MAX_SIZE * MAX_SIZE * FFGE_WIDTH];

static void test_ffge_prim_i8_unit(void)
{
	for (size_t k = 0; k < FFGE_WIDTH; k++)
		m_i8[k] = 1;
	TEST_EQ(ffge_prim_i8(m_i8, 1), 0xff);

	m_i8[3] = 0;
	TEST_EQ(ffge_prim_i8(m_i8, 1), 0b11110111);

	m_i8[6] = 0;
	TEST_EQ(ffge_prim_i8(m_i8, 1), 0b10110111);
}  

static void test_ffge_prim_i8_two_01(void)
{
	for (size_t k = 0; k < FFGE_WIDTH; k++) {
		m_i8[(0*2 + 0)*FFGE_WIDTH + k] = 0;
		m_i8[(0*2 + 1)*FFGE_WIDTH + k] = 1;
		m_i8[(1*2 + 0)*FFGE_WIDTH + k] = 1;
		m_i8[(1*2 + 1)*FFGE_WIDTH + k] = 0;
	}

	TEST_EQ(ffge_prim_i8(m_i8, 2), 0xff);
} 


static void test_ffge_prim_i8_two_02(void)
{
	for (size_t k = 0; k < FFGE_WIDTH; k++) {
		m_i8[(0*2 + 0)*FFGE_WIDTH + k] = 0;
		m_i8[(0*2 + 1)*FFGE_WIDTH + k] = 1;
		m_i8[(1*2 + 0)*FFGE_WIDTH + k] = 1;
		m_i8[(1*2 + 1)*FFGE_WIDTH + k] = 0;
	}
	m_i8[(0*2 + 1)*FFGE_WIDTH + 4] = 0;

	TEST_EQ(ffge_prim_i8(m_i8, 2), 0b11101111);
} 

static void test_ffge_prim_i8_randrank(size_t n)
{
 for (size_t rep = 0; rep < REPS; rep++) {

	uint8_t fl = 0;

	/* generate random matrix; set ref. flags, pack it */
	for (size_t k = 0; k < FFGE_WIDTH; k++) {
		size_t rnk = (xoshiro256ss_next(&RNG) % 2) == 1 ? 
			n : xoshiro256ss_next(&RNG) % n;
		if (rnk == n)
			fl |= (1 << k);
		ffge_mat_genrand_prim(m, n, rnk, 99, &RNG);
		for (size_t i = 0; i < n; i++)
			for (size_t j = 0; j < n; j++)
				m_i8[(i*n + j)*FFGE_WIDTH + k] = m[i*n + j];
	}

	TEST_EQ(ffge_prim_i8(m_i8, n), fl);
 }
}

static void test_ffge_prim_i8(void)
{
	test_ffge_prim_i8_unit();

	test_ffge_prim_i8_two_01();
	test_ffge_prim_i8_two_02();

	test_ffge_prim_i8_randrank(3);
	test_ffge_prim_i8_randrank(6);
	test_ffge_prim_i8_randrank(12);
	test_ffge_prim_i8_randrank(23);
}

static void TEST_MAIN(void)
{
	xoshiro256ss_init(&RNG, SEED);

	test_ffge_prim_i8();
}
