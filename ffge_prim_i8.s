; --------------------------------------------------------------------------- ;
; ffge_prim_i8.s: AVX512 implementation of ffge_prim.                         ;
;                                                                             ;
; Copyright 2024 Marek Miller & ⧉⧉⧉                                           ;
;                                                                             ;
; This program is free software: you can redistribute it and/or modify it     ;
; under the terms of the GNU General Public License as published by the       ;
; Free Software Foundation, either version 3 of the License, or (at your      ;
; option) any later version.                                                  ;
;                                                                             ;
; This program is distributed in the hope that it will be useful, but         ;
; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY  ;
; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    ;
; for more details.                                                           ;
;                                                                             ;
; You should have received a copy of the GNU General Public License along     ;
; with this program.  If not, see <https://www.gnu.org/licenses/>.            ;
; --------------------------------------------------------------------------- ;
[bits 64]
default rel

global ffge_prim_i8

section .rodata
	FFGE_PRIM	dq 0x7fffffff		; 2^31 - 1, a Mersenne prime

section .note.GNU-stack
section .text
	extern ffge_pivot_find_i8

;
; uint8_t ffge_prim_i8(int64_t *m, size_t n)
;
ffge_prim_i8:
	xor		rax, rax
	test		rsi, rsi
	jz		.rt0

	push		r14
	push		r13
	push		r12

	; initialize state
	mov		rax, 0xff		; rax = full-rank flags
	xor		rcx, rcx		; rcx = pv, current pivot index
	mov		rdx, rsi
	shl		rdx, 6			; rdx = size of row in bytes
	mov		r12, rdi		; r12 -> m[pv*n + pv]
	mov		r13, rdi
	add		r13, rdx
	sub		r13, 64			; r13 -> m[pv*n + n - 1]
	mov		r14, rsi
	imul		r14, rsi
	sub		r14, 1
	shl		r14, 6
	add		r14, rdi		; r14 -> m[n*n - 1]
	vpbroadcastq	zmm14, [FFGE_PRIM]
	vpxorq		zmm15, zmm15

.l0:	push		rdi
	push		rsi
	push		rdx
	push		rcx
	mov		rdx, rcx
	mov		rcx, rax
	call		ffge_pivot_find_i8 wrt ..plt
	pop		rcx
	pop		rdx
	pop		rsi
	pop		rdi

	cmp		r12, r14
	je		.rt1

	mov		r11, r12
	add		r11, rdx		; r11 -> m[i*n + pv]
	vmovdqa64	zmm0, [r12]
.l1:	vmovdqa64	zmm1, [r11]
	mov		r10, r12
	add		r10, 64			; r10 -> m[pv*n + j]
	mov		r9, r11
	add		r9, 64			; r9 -> m[i*n + j]
.l2:	vmovdqa64	zmm2, [r10]
	vmovdqa64	zmm3, [r9]

	; compute zmm3 =
	;     m[i*n + j] * m[pv*n + pv] - m[i*n + pv] * m[pv*n + j]
	vpmullq		zmm3, zmm3, zmm0
	vpmullq		zmm2, zmm2, zmm1
	vpsubq		zmm3, zmm3, zmm2

	; compute zmm3 % FFGE_PRIM
	vpmovq2m	k3, zmm3
	vpabsq		zmm4, zmm3
	vpandq		zmm5, zmm4, zmm14
	vpsraq		zmm6, zmm4, 31
	vpandq		zmm7, zmm6, zmm14
	vpaddq		zmm5, zmm5, zmm7
	vpsraq		zmm6, zmm4, 62
	vpandq		zmm7, zmm6, zmm14
	vpaddq		zmm5, zmm5, zmm7
	vpsraq		zmm6, zmm5, 31
	vpaddq		zmm5, zmm5, zmm6
	vpandq		zmm5, zmm5, zmm14
	vpsubq		zmm6, zmm5, zmm14
	vpmovq2m	k6, zmm6
	vpsubq		zmm5 {k3}, zmm15, zmm5
	vmovdqa64	zmm3, zmm15
	vmovdqa64	zmm3 {k6}, zmm5

	vmovdqa64	[r9], zmm3

	add		r9, 64
	add		r10, 64
	cmp		r10, r13
	jbe		.l2

	; zero the matrix elements below current diagonal m[pv*n + pv]
	vmovdqa64	 [r11], zmm15

	add		r11, rdx
	cmp		r11, r14
	jbe		.l1

	inc		rcx
	add		r13, rdx
	add		r12, rdx
	add		r12, 64
	jmp		.l0

.rt1:	pop		r12
	pop		r13
	pop		r14

.rt0:	ret
