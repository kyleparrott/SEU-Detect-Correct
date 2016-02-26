/* Stuff specific to the general (integer) version of the Reed-Solomon codecs
 *
 * Copyright 2003, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 */
#ifndef _RS_H
#define _RS_H

#include <stdint.h>
#include <reed_solomon.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

typedef uint32_t dword_t;

#define GFPOLY 0x201B

#define BITS_PER_SYMBOL		13
#define SYMBOL_MASK			((1 << BITS_PER_SYMBOL) - 1) /* 0x1FFF (8192 - 1) symbols per block */
#define TOTAL_SYMBOL_COUNT	((1 << BITS_PER_SYMBOL) - 1) /* 0x1FFF (8192 - 1) symbols per block */
#define PARITY_SYMBOL_COUNT	(RS_MAX_ERRORS * 2)
#define DATA_SYMBOL_COUNT	(TOTAL_SYMBOL_COUNT - PARITY_SYMBOL_COUNT)
#define PARITY_START        (DATA_SYMBOL_COUNT) /* index of the first parity symbol in data[] */
#define BITS_PER_WORD		(sizeof(word_t) * 8)
#define SYMBOL_TABLE_WORDS	(TOTAL_SYMBOL_COUNT * BITS_PER_SYMBOL / BITS_PER_WORD + 1)
#define PARITY_SYMBOL_WORDS (PARITY_SYMBOL_COUNT * BITS_PER_SYMBOL / BITS_PER_WORD + 1)

#define FCS 1
#define PRIM 1
#define IPRIM 1

#define DIVIDE_BY_BITS_PER_WORD(x) ((x) >> 4)
#define MODULO_BITS_PER_WORD(x) ((x) - word_idx * BITS_PER_WORD)
#define GET_PARTS(i) \
	register dword_t symbits = idx * BITS_PER_SYMBOL; \
	register dword_t word_idx = DIVIDE_BY_BITS_PER_WORD(symbits); \
	register dword_t bits_offset = MODULO_BITS_PER_WORD(symbits)

inline static symbol_t __attribute__((no_instrument_function, always_inline)) symbol_get(symbol_t* ptr, int idx) {
	GET_PARTS(idx);
	return ((symbol_t)(*((dword_t*)&ptr[word_idx]) >> bits_offset) & SYMBOL_MASK);
};

inline static word_t* __attribute__((no_instrument_function, always_inline)) symbol_address_offset(symbol_t* ptr, int idx) {
	return DIVIDE_BY_BITS_PER_WORD(idx * BITS_PER_SYMBOL);;
};

inline static void __attribute__((no_instrument_function, always_inline)) symbol_put(symbol_t* ptr, int idx, symbol_t sym) {
	GET_PARTS(idx);
	*((dword_t*)&ptr[word_idx]) = ((*((dword_t*)&ptr[word_idx]) & ~((dword_t)SYMBOL_MASK << bits_offset)) |
									((dword_t)(sym & SYMBOL_MASK) << bits_offset));
};

inline static symbol_t __attribute__((no_instrument_function, always_inline)) modnn(symbol_t x)
{
	while (x >= TOTAL_SYMBOL_COUNT) {
		x -= TOTAL_SYMBOL_COUNT;
		x = (x >> BITS_PER_SYMBOL) + (x & SYMBOL_MASK);
	}
	return x;
}

#ifndef _INIT_RS_C
extern symbol_t alpha_to[SYMBOL_TABLE_WORDS]; /* log lookup table */
extern symbol_t index_of[SYMBOL_TABLE_WORDS]; /* Antilog lookup table */
extern symbol_t genpoly[PARITY_SYMBOL_WORDS];  /* Generator polynomial */
#endif

#endif /* #define _RS_H */