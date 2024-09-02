# ffge-avx512

SIMD implementation of fraction-free Gaussian elimination (FFGE) over
the prime field `Z_p` for the Mersenne prime number `p = 2^31 - 1`.

## Overview

This library implements the algorithm of fraction-free Gaussian elimination
(FFGE).  You can use the library's routines e.g., to bring a square matrix
with integer coefficients to its row echelon form.  This is an efficient way
to compute the rank of a matrix.

As an improvement to standard FFGE, the functions provided here can perform
Gaussian elimination on matrices with elements from the prime field `Z_p`
for `p = 2^31 - 1`.  The set `Z_p` contains almost all 32-bit-wide signed
integers, with the exception of `p`, `-p` and `-p-1`.  Because the computation
over `Z_p` avoids integer division altogether, it is suitable for vectorization
and can be considerably faster that the standard FFGE. For a matrix whose rank
is expected to be less than `p`, the task to decide whether the matrix is
invertible can be executed on a CPU supporting the AVX-512 instruction set
extension up to 8 times faster than a scalar implementation of the same
algorithm.

That sort of problem was the motivation to develop the library and to implement
the SIMD version of its core routines in x86-64 assembly.

## Using the library

The library's main functionality is defined in the header file
[`ffge.h`](./ffge.h).  The file also contains the documentation of the provided
interface.

### Compiling the source code

To obtain the source code, clone the public git repository:

```bash
git clone https://github.com/marek-miller/ffge-avx512
cd ffge-avx512
```

The toolchain necessary to compile the sources consists of:

* [NASM](https://www.nasm.us) assembler,
* C compiler supporting C23 standard, e.g. GCC version 14 and above.

If you use `gcc` version 13, you should be able to compile the sources with
no issues if you modify the `-std` flag in [`Makefile`](./Makefile)

```makefile
CFLAGS	+= -std=c2x	# etc., instead of -std=c23
```

To compile the library, run:

```bash
make all
```

### Testing

Assuming your CPU supports AVX-512, run the library's test suite by typing:

```bash
make check
```

### Installation

No installation mechanism has been provided yet.  Simply copy the static
library file `libffge.a` and the header `ffge.h` to a location appropriate
for your system.

### API documentation

The interface is documented in the header file [`ffge.h`](./ffge.h).


