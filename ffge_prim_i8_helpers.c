/* -------------------------------------------------------------------------- *
 * ffge_prim_i8_helpers.c: Helper functions for SIMD implementation of FFGE.  *
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
#include <stddef.h>
#include <stdint.h>

#include "ffge.h"

/* Find the next row with non-zero element at pivot column pc. Swap rows.
 *
 * The matrix m is in fact packed FFGE_WIDTH matrices, the argument k
 * is the matrix index.
 *
 * Returns the full-rank flags passed fl (see docstring for ffge_prim_i8)
 * with the k-th flag zeroed, if the corresponding matrix is singular.
 */
uint64_t ffge_pivot_find_i8(int64_t *m, size_t n, size_t pv, uint64_t fl)
{
	for (size_t k = 0; k < FFGE_WIDTH; k++) {
		size_t i = pv;
		while (i < n && m[(i*n + pv)*FFGE_WIDTH + k] == 0)
			i++;

		if (i == n) {
			fl &= ~(1 << k);
			continue;
		}
		if (i > pv)			/* swap rows */
			for (size_t j = pv; j < n; j++) {
				int64_t *x, *y, zz;
				zz = *(x = m + (pv*n + j)*FFGE_WIDTH + k);
				*x = *(y = m + ( i*n + j)*FFGE_WIDTH + k);
				*y = zz;
			}
	}

	return fl;
}
