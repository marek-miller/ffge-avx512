; --------------------------------------------------------------------------- ;
; ffge_prim_i8.s: AVX512 implementation of ffge_prim.                         ;
;                                                                             ;
; Copyright 2024 ⧉⧉⧉                                                          ;
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
	FFGE_PRIM	dq 0x7fffffff

section .note.GNU-stack
section .text
	extern ffge_pivot_find_i8

;
; int64_t ffge_prim_i8(int64_t *m, size_t n)
;
ffge_prim_i8:
	push		r15
	push		r14
	push		r13
	push		r12
	push		rbx

	; store function arguments
	mov		rbx, rdi		; int64_t *m
	mov		r13, rsi		; size_t n

	; initialize state
	mov		rax, 0xff		; full-rank flags
	mov		r12, rdi		; current diagonal element
	xor		r14, r14		; size_t pv, pivot column/row
	mov		r15, r13
	imul		r15, 64			; size of row in bytes
	vpbroadcastq	zmm14, [FFGE_PRIM]
	vpxorq		zmm15, zmm15

.l0:	mov		rdi, rbx
	mov		rsi, r13
	mov		rdx, r14
	mov		rcx, rax
	call		ffge_pivot_find_i8

	mov		r8, r14
	add		r8, 1
	cmp		r8, r13
	jae		.rt

	mov		r11, r8
	imul		r11, r13
	add		r11, r14
	imul		r11, 64
	add		r11, rbx

	vmovdqa64	zmm0, [r12]
.l1:	vmovdqa64	zmm1, [r11]

	mov		rcx, r14
	add		rcx, 1

	mov		r9, r8
	imul		r9, r13
	add		r9, rcx
	imul		r9, 64
	add		r9, rbx

	mov		r10, r14
	imul		r10, r13
	add		r10, rcx
	imul		r10, 64
	add		r10, rbx

.l2:	vmovdqa64	zmm2, [r10]
	vmovdqa64	zmm3, [r9]

	vpmullq		zmm3, zmm3, zmm0
	vpmullq		zmm2, zmm2, zmm1
	vpsubq		zmm3, zmm3, zmm2

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
	vmovdqa64	zmm6, zmm15
	vmovdqa64	zmm6 {k6}, zmm5
	vmovdqa64	[r9], zmm6

	add		r9, 64
	add		r10, 64
	inc		rcx
	cmp		rcx, r13
	jb		.l2

	vmovdqa64	 [r11], zmm15

	add		r11, r15
	inc		r8
	cmp		r8, r13
	jb		.l1

	add		r12, r15
	add		r12, 64
	inc		r14
	cmp		r14, r13
	jb		.l0

.rt:	pop		rbx
	pop		r12
	pop		r13
	pop		r14
	pop		r15

	vzeroupper
	ret
