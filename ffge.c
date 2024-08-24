/* -------------------------------------------------------------------------- *
 * ffge.c: Fraction-free Gaussian elimination.                                *
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

int ffge_pivot_find(int64_t *m, size_t n, size_t pr, size_t pc)
{
	size_t i = pr;
	while (i < n && m[i*n + pc] == 0)
		i++;

	if (i == n)
		return -1;
	if (i > pr)
		for (size_t j = pc; j < n; j++) {
			int64_t *x, *y, zz;

			zz = *(x = m + pr*n + j);
			*x = *(y = m +  i*n + j);
			*y = zz;
		}

	return 0;
}

size_t ffge(int64_t *m, size_t n)
{
	int64_t dv = 1;
	size_t pc, pr = 0;	/* pivot column, row */
	for (pc = 0; pc < n; pc++) {
		if (ffge_pivot_find(m, n, pr, pc) < 0)
			continue;

		const int64_t m_rc = m[pr*n + pc];
		for (size_t i = pr + 1; i < n; i++) {
			const int64_t m_ic = m[i*n + pc];
			for (size_t j = pc + 1; j < n; j++)
				m[i*n + j] =
				(m[i*n + j] * m_rc - m[pr*n + j] * m_ic) / dv;

			m[i*n + pc] = 0;
		}
		dv = m_rc;
		pr++;
	}

	return pr;
}

size_t ffge_prim(int64_t *m, size_t n)
{
	size_t pc, pr = 0;	/* pivot column, row */
	for (pc = 0; pc < n; pc++) {
		if (ffge_pivot_find(m, n, pr, pc) < 0)
			continue;

		const int64_t m_rc = m[pr*n + pc];
		for (size_t i = pr + 1; i < n; i++) {
			const int64_t m_ic = m[i*n + pc];
			for (size_t j = pc + 1; j < n; j++)
				m[i*n + j] =
				(m[i*n + j] * m_rc - m[pr*n + j] * m_ic) %
					FFGE_PRIM;

			m[i*n + pc] = 0;
		}
		pr++;
	}

	return pr;
}

