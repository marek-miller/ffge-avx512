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

ffge_prim_i8:
	push	r15
	push	r14
	push	r13
	push	r12
	push	rdx			; uint8_t *fl

	mov	r12, rdi		; int64_t *m
	mov	r13, rsi		; size_t n
	xor	r14, r14		; size_t pv
	mov	r15, r13
	imul	r15, 64
	mov	rax, 0xff		; full-rank flags

	vpbroadcastq zmm14, [FFGE_PRIM]
	vpxorq	zmm15, zmm15

.l0:	mov	rdi, r12
	mov	rsi, r13
	mov	rdx, r14
	mov	rcx, rax
	call	ffge_pivot_find_i8

	mov	rdi, r14
	imul	rdi, r13
	add	rdi, r14
	imul	rdi, 64
	add	rdi, r12
	vmovdqa64 zmm0, [rdi]

	mov	r8, r14
	add	r8, 1
	cmp	r8, r13
	jae	.l8a

	mov	rdi, r8
	imul	rdi, r13
	add	rdi, r14
	imul	rdi, 64
	add	rdi, r12

.l8:
	vmovdqa64 zmm1, [rdi]

	mov	r9, r14
	add	r9, 1
	cmp	r9, r13
	jae	.l9a

	mov	r10, r14
	imul	r10, r13
	add	r10, r9
	imul	r10, 64
	add	r10, r12

	mov	r11, r8
	imul	r11, r13
	add	r11, r9
	imul	r11, 64
	add	r11, r12
.l9:
	vmovdqa64 zmm2, [r10]
	vmovdqa64 zmm3, [r11]

	vpmullq	zmm3, zmm0, zmm3
	vpmullq	zmm2, zmm1, zmm2
	vpsubq	zmm3, zmm3, zmm2

	vpmovq2m k3, zmm3
	vpabsq	zmm4, zmm3

	vpandq	zmm5, zmm4, zmm14
	vpsraq	zmm6, zmm4, 31
	vpandq	zmm6, zmm6, zmm14
	vpsraq	zmm7, zmm4, 62
	vpandq	zmm7, zmm7, zmm14
	vpaddq	zmm5, zmm5, zmm6
	vpaddq	zmm5, zmm5, zmm7
	vpsraq	zmm6, zmm5, 31
	vpaddq	zmm5, zmm5, zmm6
	vpandq	zmm5, zmm5, zmm14

	vpsubq	zmm6, zmm5, zmm14
	vpmovq2m k6, zmm6

	vpsubq	zmm5 {k3}, zmm15, zmm5
	vmovdqa64 zmm3, zmm15
	vmovdqa64 zmm3 {k6}, zmm5
	vmovdqa64 [r11], zmm3

	add	r10, 64
	add	r11, 64
	inc	r9
	cmp	r9, r13
	jb	.l9
.l9a:
	vmovdqa64 [rdi], zmm15
	add	rdi, r15
	inc	r8
	cmp	r8, r13
	jb	.l8
.l8a:
	inc	r14
	cmp	r14, r13
	jb	.l0

	pop	rdx
	pop	r12
	pop	r13
	pop	r14
	pop	r15

	test	rdx, rdx
	jz	.rt
	mov	[rdx], al
.rt:	cmp	al, 0xff
	sete	al
	ret

