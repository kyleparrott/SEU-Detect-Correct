/* Stuff specific to the general (integer) version of the Reed-Solomon codecs
 *
 * Copyright 2003, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */

#include <stdint.h>

#define MODNN(x) modnn(rs, x)

#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define MM (rs->mm)
#define NN (rs->nn)
#define ALPHA_TO (rs->alpha_to)
#define INDEX_OF (rs->index_of)
#define GENPOLY (rs->genpoly)
#define NROOTS (rs->nroots)
#define FCR (rs->fcr)
#define PRIM (rs->prim)
#define IPRIM (rs->iprim)
#define PAD (rs->pad)
#define A0 (NN)

/* Reed-Solomon codec control block */
typedef struct rs {
	int mm;             /* Bits per symbol */
	int nn;             /* Symbols per block (= (1<<mm)-1) */
	uint16_t *alpha_to;   /* log lookup table */
	uint16_t *index_of;   /* Antilog lookup table */
	uint16_t *genpoly;    /* Generator polynomial */
	int nroots;         /* Number of generator roots = number of parity symbols */
	int fcr;            /* First consecutive root, index form */
	int prim;           /* Primitive element, index form */
	int iprim;          /* prim-th root of 1, index form */
	int pad;            /* Padding bytes in shortened block */
} rs_t;


/* General purpose RS codec, integer symbols */
void encode_rs(rs_t* rs, uint16_t *data, uint16_t *parity);
int decode_rs(rs_t* rs, uint16_t *data);
rs_t* init_rs(int symsize, int gfpoly, int fcr, int prim, int nroots, int pad);
void free_rs(rs_t* rs);

static inline unsigned int modnn(rs_t* rs, unsigned int x) {
	while (x >= rs->nn) {
		x -= rs->nn;
		x = (x >> rs->mm) + (x & rs->nn);
	}
	return x;
}
