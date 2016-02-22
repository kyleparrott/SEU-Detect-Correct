/* Reed-Solomon encoder
 * Copyright 2003, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#include <string.h>
#include "rs.h"

void encode_rs(rs_t* rs, uint16_t *data, uint16_t *parity)
{
	int i, j;
	uint16_t feedback;

	memset(parity, 0, NROOTS * sizeof(uint16_t));

	for (i = 0; i < NN - NROOTS - PAD; i++) {
		feedback = INDEX_OF[data[i] ^ parity[0]];
		if (feedback != A0) { /* feedback term is non-zero */
			for (j = 1; j < NROOTS; j++) {
				parity[j] ^= ALPHA_TO[MODNN(feedback + GENPOLY[NROOTS - j])];
			}
		}
		/* Shift */
		memmove(&parity[0], &parity[1], sizeof(uint16_t) * (NROOTS - 1));
		if (feedback != A0) {
			parity[NROOTS - 1] = ALPHA_TO[MODNN(feedback + GENPOLY[0])];
		}
		else {
			parity[NROOTS - 1] = 0;
		}
	}

}
