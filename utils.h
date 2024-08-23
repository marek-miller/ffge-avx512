/* -------------------------------------------------------------------------- *
 * utils.h: Matrix utilities.                                                 *
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
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

#include "xoshiro256ss.h"

void ffge_mat_print(int64_t *m, size_t n);

/* Generate a random square matrix of size n and having rank equal to rnk <= n.
 *
 * The element of the matrix are numbers from Z_p prime field for
 * p = FFGE_PRIM.  Also, both n and rnk are assumed to be less than FFGE_PRIM.
 *
 * Perform at most rd rounds of elementary matrix row and column operations.
 *
 */
void ffge_mat_genrand_prim(int64_t *m, size_t n, size_t rnk, size_t rd,
			struct xoshiro256ss *rng);

#endif /* UTILS_H */
