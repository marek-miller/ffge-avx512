/* -------------------------------------------------------------------------- *
 * utils.c: Matrix utilities.                                                 *
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

#include "ffge.h"
#include "utils.h"
#include "xoshiro256ss.h"

void ffge_mat_genrand_prim(int64_t *m, size_t n, size_t rnk, size_t rd,
			struct xoshiro256ss *rng)
{
	for (size_t i = 0; i < n; i++)
		for (size_t j = 0; j < n; j++)
			m[i*n + j] = (i == j && i < rnk) ? 1 : 0;

	while (rd-- > 0) {
		size_t r1, r2, c1, c2, rc[8];	/* random rows, columns */
		int ss[2];			/* random signs */

		for (size_t i = 0; i < 8; i++)
			rc[i] = xoshiro256ss_next(rng) % n;
		for (size_t i = 0; i < 2; i++)
			ss[i] = (xoshiro256ss_next(rng) % 2) * 2 - 1;

		/* swap rows */
		r1 = rc[0]; r2 = rc[1];
		if (r1 != r2)
			for (size_t j = 0; j < n; j++) {
				int64_t *x, *y, zz;
				zz = *(x = m + r1*n + j);
				*x = *(y = m + r2*n + j);
				*y = zz;
			}

		/* swap columns */
		c1 = rc[2]; c2 = rc[3];
		if (c1 != c2)
			for (size_t i = 0; i < n; i++) {
				int64_t *x, *y, zz;
				zz = *(x = m + i*n + c1);
				*x = *(y = m + i*n + c2);
				*y = zz;
			}

		/* add rows */
		r1 = rc[4]; r2 = rc[5];
		if (r1 != r2)
			for (size_t j = 0; j < n; j++)
				m[r1*n + j] =
				(m[r1*n + j] + ss[0] * m[r2*n + j]) % FFGE_PRIM;

		/* add columns */
		c1 = rc[6]; c2 = rc[7];
		if (c1 != c2)
			for (size_t i = 0; i < n; i++)
				m[i*n + c1] =
				(m[i*n + c1] + ss[1] * m[i*n + c2]) % FFGE_PRIM;
	}
}
