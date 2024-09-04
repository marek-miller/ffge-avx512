/* -------------------------------------------------------------------------- *
 * benchmark.c: Measure performance of provided functions.                    *
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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "bench.h"
#include "ffge.h"
#include "utils.h"
#include "xoshiro256ss.h"

#define SEED UINT64_C(9482110)
static struct xoshiro256ss RNG;

#define SIZE (12)
static int64_t m[SIZE*SIZE];
static alignas(64) int64_t m_i8[SIZE*SIZE * FFGE_WIDTH];

#define REPS (9999UL)
static double bench_avgmicros(struct bench *b)
{
	return (double)b->nanos / b->reps / 1000L;
}

static int genrand_mt(void *)
{
	/* Generate random matrices: 50% full-rank, 50% singular. */
	size_t rnk = (xoshiro256ss_next(&RNG) % 2) == 1 ?
		SIZE : xoshiro256ss_next(&RNG) % SIZE;
	ffge_mat_genrand_prim(m, SIZE, rnk, 99, &RNG);

	return 0;
}

static int rank12(void *)
{
	genrand_mt(nullptr);
	ffge(m, SIZE);

	return 0;
}

static int rank12_prim(void *)
{
	genrand_mt(nullptr);
	ffge_prim(m, SIZE);

	return 0;
}

static int genrand_mt_i8(void *)
{
	/* Generate random matrices: 50% full-rank, 50% singular. */
	for (size_t k = 0; k < FFGE_WIDTH; k++) {
		genrand_mt(nullptr);
		for (size_t i = 0; i < SIZE; i++)
			for (size_t j = 0; j < SIZE; j++)
				m_i8[(i*SIZE + j)*FFGE_WIDTH + k] =
					m[i*SIZE + j];
	}

	return 0;
}

static int rank12_prim_i8(void *)
{
	genrand_mt_i8(nullptr);
	ffge_prim_i8(m_i8, SIZE);

	return 0;
}

int main(int, char **)
{
	xoshiro256ss_init(&RNG, SEED);

	double t_genrand, t_genrand_i8;
	struct bench b;

	bench_mark(&b, REPS, genrand_mt, nullptr);
	t_genrand = bench_avgmicros(&b);
	printf("genrand_mt:      %.3f μs\n", t_genrand);

	bench_mark(&b, REPS, rank12, nullptr);
	printf("rank12:          %.3f μs", bench_avgmicros(&b));
	printf(" (excl. genrand_mt: %.3f μs)\n",
				 bench_avgmicros(&b) - t_genrand);

	bench_mark(&b, REPS, rank12_prim, nullptr);
	printf("rank12_prim:     %.3f μs", bench_avgmicros(&b));
	printf(" (excl. genrand_mt: %.3f μs)\n",
				 bench_avgmicros(&b) - t_genrand);

	bench_mark(&b, REPS, genrand_mt_i8, nullptr);
	t_genrand_i8 = bench_avgmicros(&b);
	printf("genrand_mt_i8:   %.3f μs\n", t_genrand_i8);

	bench_mark(&b, REPS, rank12_prim_i8, nullptr);
	printf("rank12_prim_i8:  %.3f μs", bench_avgmicros(&b));
	printf(" (excl. genrand_mt_i8, avg.: %.3f μs)\n",
		(bench_avgmicros(&b) - t_genrand_i8) / FFGE_WIDTH);

	return 0;
}
