#ifndef _REED_SOLOMON_H
#define _REED_SOLOMON_H


#define RS_MAX_ERRORS		8 /* Tolerate up to 8 errors per block */

typedef uint16_t symbol_t; /* the lower BITS_PER_SYMBOL bits are valid */
typedef uint16_t word_t;

int __attribute__((no_instrument_function)) decode_rs1(symbol_t *data, word_t offsets[RS_MAX_ERRORS], word_t values[RS_MAX_ERRORS]);
int __attribute__((no_instrument_function)) decode_rs2(symbol_t *data, word_t offsets[RS_MAX_ERRORS], word_t values[RS_MAX_ERRORS]);
int __attribute__((no_instrument_function)) decode_rs3(symbol_t *data, word_t offsets[RS_MAX_ERRORS], word_t values[RS_MAX_ERRORS]);

#endif /* _REED_SOLOMON_H */
