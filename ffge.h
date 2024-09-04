/* -------------------------------------------------------------------------- *
 * ffge.h: Fraction-free Gaussian elimination.                                *
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
#ifndef FFGE_H
#define FFGE_H

#include <stddef.h>
#include <stdint.h>

#define FFGE_PRIM (0x7FFFFFFFL)		/* 2^31 - 1, a Mersenne prime */
#define FFGE_WIDTH (8)			/* Width of the SIMD vector */

/* Perform in-place FFGE of a square matrix m of size n.
 *
 * The martix is represented as a continuous array of rows, i.e.
 * if i, j = 0, 1, 2, ..., n-1, then the matrix element m_ij is stored at:
 *
 *     m[i*n + j]
 *
 * The function returns the rank of the matrix m.
 */
size_t ffge(int64_t *m, size_t n);

/* Perform in-place FFGE of a square matrix m of size n over the prime
 * field Z_p for p = FFGE_PRIM.
 *
 * Assume n < FFGE_PRIM.
 *
 * The martix is represented as a continuous array of rows, i.e.
 * if i, j = 0, 1, 2, ..., n-1, then the matrix element m_ij is stored at:
 *
 *     m[i*n + j]
 *
 * The function returns the rank of the matrix m (modulo FFGE_PRIM).
 */
size_t ffge_prim(int64_t *m, size_t n);

/* Perform in-place FFGE of FFGE_WIDTH packed square matrices of size n,
 * over the prime field Z_p for p = FFGE_PRIM = 2^31 - 1.
 *
 * Assume n < FFGE_PRIM.
 *
 * The matrices are represented as a continuous array of packed rows, i.e. if
 * i, j = 0, 1, 2, ... n-1, and k = 0, 1, ..., FFGE_WIDTH-1, then
 * the i,j-th element of the k-th matrix, (m_k)_{i,j} is stored at
 *
 *     m[(i*n + j)*FFGE_WIDTH + k]
 *
 * This function is fast, but the result is correct only for those packed
 * matrices that are full-rank.  The matrices that are singular (and only
 * those) are destroyed as a result of this function.
 *
 * The matrix m must be aligned to the 64 byte boundary.  For example, for
 * a square matrix of size SIZE, one can alocate static storage explicitly by:
 *
 *     alignas(64) int64_t m[SIZE * SIZE * FFGE_WIDTH];
 *
 * The function returns a set of flags indicating which matrices have full rank,
 * i.e. for
 *
 *     uint8_t fl = ffge_prim_i8(m, n);
 *
 * the number
 *
 *     (*f >> k) & 1
 *
 * is equal to 1 if k-th matrix has full rank, k = 0, 1, ..., FFGE_WIDTH-1.
 */
uint8_t ffge_prim_i8(int64_t *m, size_t n);

#endif /* FFGE_H */
