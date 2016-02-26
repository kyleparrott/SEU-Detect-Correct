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


/* #define RS_DUMP_BLOCKS */
/* #define RS_BUILD_TABLES */
/* #define DEBUG 2 */



int exercise_int(void);

#ifdef RS_DUMP_BLOCKS
#define RS_DUMP_SYMBOLS
void dump_blocks(word_t* block, word_t* errblock);
#endif

#ifdef RS_BUILD_TABLES
#define RS_DUMP_SYMBOLS
void dump_tables(void);
#endif

#ifdef RS_DUMP_SYMBOLS
void dump_symbols(word_t* ptr);
#endif

int main()
{
	srandom(time(NULL));

	printf("Testing (%d,%d) code...\n", TOTAL_SYMBOL_COUNT, DATA_SYMBOL_COUNT);
	exercise_int();

	return(0);
}

int exercise_int(void)
{
	init_rs();

#ifdef RS_BUILD_TABLES
	dump_tables();
#else
	word_t block[SYMBOL_TABLE_WORDS], tblock[SYMBOL_TABLE_WORDS];
	int errlocs[TOTAL_SYMBOL_COUNT];
	int i;
	int errors = 0;
	int derrors;
	symbol_t errval;
	int errloc;

#ifndef RS_DUMP_BLOCKS
	/* Test up to the error correction capacity of the code */
	for (errors = 0; errors <= RS_MAX_ERRORS; errors++) {
#endif /* RS_DUMP_BLOCKS */

		/* Load block with random data and encode */
		for (i = 0; i < DATA_SYMBOL_COUNT; i++) {
			symbol_put(block, i, random() & SYMBOL_MASK);
		}

		encode_rs(block);

		/* Make temp copy, seed with errors */
		memcpy(tblock, block, sizeof(block));
		memset(errlocs, 0, sizeof(errlocs));

		for (i = 0; i < errors; i++) {
			do {
				errval = random() & SYMBOL_MASK;
			} while (errval == 0); /* Error value must be nonzero */

			do {
				errloc = random() % TOTAL_SYMBOL_COUNT;
			} while (errlocs[errloc] != 0); /* Must not choose the same location twice */

			errlocs[errloc] = 1;

			symbol_put(tblock, errloc, symbol_get(tblock, errloc) ^ errval);
		}

#ifdef RS_DUMP_BLOCKS
dump_blocks(block, tblock);
#else
		/* Decode the errored block */
		derrors = decode_rs(tblock);

		if (derrors != errors) {
			printf("(%d,%d) decoder says %d errors, true number is %d\n", TOTAL_SYMBOL_COUNT, DATA_SYMBOL_COUNT, derrors, errors);
		}

		if (memcmp(tblock, block, sizeof(tblock)) != 0) {
			for (i = 0; i < TOTAL_SYMBOL_COUNT; i++) {
				if (symbol_get(tblock, i) ^ symbol_get(block, i)) {
					printf("(%d,%d) error at %d\n", TOTAL_SYMBOL_COUNT, DATA_SYMBOL_COUNT, i);
					errors++;
				}
			}
			printf("\n");
		}
	}
#endif /* RS_DUMP_BLOCKS */
#endif /* RS_BUILD_TABLES */
	return 0;
}

#ifdef RS_DUMP_BLOCKS
void dump_blocks(word_t* block, word_t* errblock)
{
	printf("word_t input_block[SYMBOL_TABLE_WORDS] = {\n");
	dump_symbols(block);

	printf("word_t error_block[SYMBOL_TABLE_WORDS] = {\n");
	dump_symbols(errblock);
}
#endif

#ifdef RS_BUILD_TABLES
void dump_tables(void)
{
	printf("word_t alpha_to[SYMBOL_TABLE_WORDS] = {\n");
	dump_symbols(word_t*)alpha_to);

	printf("word_t index_of[SYMBOL_TABLE_WORDS] = {\n");
	dump_symbols(word_t*)index_of);

	ptr = (word_t*)genpoly;
	printf("word_t genpoly[PARITY_SYMBOL_WORDS] = {\n");
	i=0;
	while(i < (PARITY_SYMBOL_COUNT - 1))
	{
		printf("\t");
		do {
			printf("0x%04X, ", symbol_get(ptr, i));
			i++;
		} while (((i + 1) % 8) != 0 && i < (PARITY_SYMBOL_COUNT - 1));
		if (i < (PARITY_SYMBOL_COUNT - 1))
		{
			printf("0x%04X,\n", symbol_get(ptr, i));
			i++;
		}
	}
	printf("0x%04X\n};\n\n", symbol_get(ptr, i));
}

#endif /* RS_BUILD_TABLES */

#ifdef RS_DUMP_SYMBOLS
void dump_symbols(word_t* ptr) {
	int i;

	i=0;
	while(i < (TOTAL_SYMBOL_COUNT - 1))
	{
		printf("\t");
		do {
			printf("0x%04X, ", symbol_get(ptr, i));
			i++;
		} while (((i + 1) % 8) != 0 && i < (TOTAL_SYMBOL_COUNT - 1));
		if (i < (TOTAL_SYMBOL_COUNT - 1))
		{
			printf("0x%04X,\n", symbol_get(ptr, i));
			i++;
		}
	}
	printf("0x%04X\n};\n\n", symbol_get(ptr, i));
}

#endif
