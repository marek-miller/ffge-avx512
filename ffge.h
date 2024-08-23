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
 * If rnk is not nullptr, then the rank of m is stored at the address pointed
 * to by rnk.
 *
 * The function returns 1, if the matrix m has rank equal to n, otherwise
 * it returns 0.
 */
int ffge(int64_t *m, size_t n, size_t *rnk);

/* Perform in-place FFGE of a square matrix m of size n over the prime
 * field Z_p for p = FFGE_PRIM.
 *
 * The martix is represented as a continuous array of rows, i.e.
 * if i, j = 0, 1, 2, ..., n-1, then the matrix element m_ij is stored at:
 *
 *     m[i*n + j]
 *
 * If rnk is not nullptr, then the rank (modulo FFGE_PRIM) of m is stored
 *  at the address pointed to by rnk.
 *
 * The function returns 1, if the matrix m has rank equal to n for
 * n < FFGE_PRIM; if the matrix is singular over Z_p, the function returns 0,
 * in case of error (e.g. invalid matrix elements), it returns -1.
 */
int ffge_prim(int64_t *m, size_t n, size_t *rnk);

/* Perform in-place FFGE of FFGE_WIDTH packed square matrices of size n,
 * over the prime field Z_p for p = FFGE_PRIM = 2^31 - 1.
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
 * If fl is not nullptr, the it will hold a set of flags indicating which
 * matrices have full rank, i.e.
 *
 *     (*fl >> k) & 1
 *
 * is equal to 1 if k-th matrix has full rank, k = 0, 1, ..., FFGE_WIDTH-1.
 *
 * The matrix m must be aligned to the 64 byte boundary.  For example, for
 * a square matrix of size SIZ, one can alocate static storage explicitly by:
 *
 *     alignas(64) uint64_t m[SIZ*SIZ];
 *
 * The function returns 1 if at least one of the matrices has full rank,
 * otherwise it returns 0 if all matrices are singular or -1 in case of
 * an error (e.g. invalid matrix elements).
 */
int ffge_prim_i8(int64_t *m, size_t n, uint8_t *fl);

#endif /* FFGE_H */
