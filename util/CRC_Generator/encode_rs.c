/* Reed-Solomon encoder
 * Copyright 2003, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#include "rs.h"

void encode_rs(symbol_t* data, symbol_t* parity)
{
	int i, j;
	symbol_t feedback;
	symbol_t parity_words[PARITY_SYMBOL_COUNT];

	for(i=0; i<PARITY_SYMBOL_COUNT; i++) {
		parity_words[i] = 0;
	}

	for (i = 0; i < DATA_SYMBOL_COUNT; i++) {
		feedback = symbol_get(index_of, symbol_get(data, i) ^ parity_words[0]);
		if (feedback != SYMBOL_MASK) { /* feedback term is non-zero */
			for (j = 1; j < PARITY_SYMBOL_COUNT; j++) {
				parity_words[j] ^= symbol_get(alpha_to, modnn(feedback + symbol_get(genpoly, PARITY_SYMBOL_COUNT - j)));
			}
		}
		/* Shift */
		for(j=0; j < PARITY_SYMBOL_COUNT - 1; j++) {
			parity_words[j] = parity_words[j+1];
		}
		if (feedback != SYMBOL_MASK) {
			parity_words[PARITY_SYMBOL_COUNT - 1] = symbol_get(alpha_to, modnn(feedback + symbol_get(genpoly, 0)));
		}
		else {
			parity_words[PARITY_SYMBOL_COUNT - 1] = 0;
		}
	}

	for(i=0; i<PARITY_SYMBOL_COUNT; i++) {
		symbol_put(parity, i, parity_words[i]);
	}
}
