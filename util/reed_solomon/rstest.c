/* Test the Reed-Solomon codecs
 * for various block sizes and with random data and random error patterns
 *
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "rs.h"


struct etab {
	int symsize;
	int genpoly;
	int fcs;
	int prim;
	int nroots;
	int ntrials;
} Tab = { 16, 0x1100b, 1,   1, 32, 1 };

int exercise_int(struct etab *e);

int main()
{
	int i;

	srandom(time(NULL));

	for (i = 0; Tab.symsize != 0; i++) {
		int nn, kk;

		nn = (1 << Tab.symsize) - 1;
		kk = nn - Tab.nroots;
		printf("Testing (%d,%d) code...\n", nn, kk);
		exercise_int(&Tab);
	}
	exit(0);
}

int exercise_int(struct etab *e)
{
	unsigned int nn = 0xFFFF;
	uint16_t block[nn], tblock[nn];
	uint16_t errlocs[nn];
	int i;
	uint16_t errors;
	uint16_t derrors, kk;
	uint16_t errval, errloc;
	rs_t* rs;

	/* Compute code parameters */
	kk = nn - e->nroots;

	rs = init_rs(e->symsize, e->genpoly, e->fcs, e->prim, e->nroots, 0);
	if (rs == NULL) {
		printf("init_rs() failed!\n");
		return -1;
	}

	/* Test up to the error correction capacity of the code */
	for (errors = 0; errors <= e->nroots / 2; errors++) {

		/* Load block with random data and encode */
		for (i = 0; i < kk; i++) {
			block[i] = random() & nn;
		}
		memcpy(tblock, block, sizeof(block));
		encode_rs(rs, block, &block[kk]);

		/* Make temp copy, seed with errors */
		memcpy(tblock, block, sizeof(block));
		memset(errlocs, 0, sizeof(errlocs));

		for (i = 0; i < errors; i++) {
			do {
				errval = random() & nn;
			} while (errval == 0); /* Error value must be nonzero */

			do {
				errloc = random() % nn;
			} while (errlocs[errloc] != 0); /* Must not choose the same location twice */

			errlocs[errloc] = 1;

			tblock[errloc] ^= errval;
		}

		/* Decode the errored block */
		derrors = decode_rs(rs, tblock);

		if (derrors != errors) {
			printf("(%d,%d) decoder says %d errors, true number is %d\n", nn, kk, derrors, errors);
		}

		if (memcmp(tblock, block, sizeof(tblock)) != 0) {
			for (i = 0; i < nn; i++) {
				if (tblock[i] ^ block[i]) {
					printf("(%d,%d) error at %d\n", nn, kk, i);
					errors++;
				}
			}
			printf("\n");
		}
	}

	free_rs(rs);
	return 0;
}
