/* Initialize a RS codec
 *
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */

#define _INIT_RS_C
#include <stdlib.h>
#include <stdio.h>

#include "rs.h"

symbol_t alpha_to[SYMBOL_TABLE_WORDS]; /* log lookup table */
symbol_t index_of[SYMBOL_TABLE_WORDS]; /* Antilog lookup table */
symbol_t genpoly[PARITY_SYMBOL_WORDS];  /* Generator polynomial */

/* Initialize a Reed-Solomon codec
 * symsize = symbol size, bits
 * gfpoly = Field generator polynomial coefficients
 * fcr = first root of RS code generator polynomial, index form
 * prim = primitive element to generate polynomial roots
 * nroots = RS code generator polynomial degree (number of roots)
 * pad = padding bytes at front of shortened block
 */
void init_rs(void)
{

#ifdef RS_BUILD_TABLES
	int i, j;
	word_t sr, root;

		/* Generate Galois field lookup tables */
	symbol_put(index_of, 0, SYMBOL_MASK);   /* log(zero) = -inf */
	symbol_put(alpha_to, TOTAL_SYMBOL_COUNT, 0);   /* alpha**-inf = 0 */
	sr = 1;
	for (i = 0; i < TOTAL_SYMBOL_COUNT; i++) {
		symbol_put(index_of, sr, i);
		symbol_put(alpha_to, i, sr);
		sr <<= 1;
		if (sr & (1 << BITS_PER_SYMBOL)) {
			sr ^= GFPOLY;
		}
		sr &= SYMBOL_MASK;
	}

	/* Form RS code generator polynomial from its roots */
	symbol_put(genpoly, 0, 1);
	for (i = 0, root = FCS * PRIM; i < PARITY_SYMBOL_COUNT; i++, root += PRIM) {
		symbol_put(genpoly, i + 1, 1);

		/* Multiply genpoly[] by  @**(root + x) */
		for (j = i; j > 0; j--) {
			if (symbol_get(genpoly, j) != 0) {
				symbol_put(genpoly, j, symbol_get(genpoly, j - 1) ^ symbol_get(alpha_to, modnn(symbol_get(index_of, symbol_get(genpoly, j)) + root)));
			}
			else {
				symbol_put(genpoly, j, symbol_get(genpoly, j - 1));
			}
		}
		/* genpoly[0] can never be zero */
		symbol_put(genpoly, 0, symbol_get(alpha_to, modnn(symbol_get(index_of, symbol_get(genpoly, 0)) + root)));
	}
	/* convert genpoly[] to index form for quicker encoding */
	for (i = 0; i <= PARITY_SYMBOL_COUNT; i++) {
		symbol_put(genpoly, i, symbol_get(index_of, symbol_get(genpoly, i)));
	}

#endif /* RS_BUILD_TABLES */
}
