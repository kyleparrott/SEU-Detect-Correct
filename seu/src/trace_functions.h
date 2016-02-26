#ifndef _TRACE_FUNCTIONS_H
#define _TRACE_FUNCTIONS_H

typedef uint32_t (*CRCCheck_ptr_t)( struct block_header* blk_ptr );

#define BL_16K (1024 * 16)
#define BL_64K (1024 * 64)

/* NOTE: blockCRC MUST be the first member of this struct! */

typedef struct block_header {
    uint32_t blockCRC;					/*  4 bytes */
    uint32_t block_length;				/*  4 bytes */
	uint16_t reed_solomon_parity[32];   /* 64 bytes */
} block_header_t;

typedef block_header* block_header_ptr_t;
typedef uint32_t* uint32_ptr_t;

#ifndef NULL
#define NULL (0)
#endif

#define K_BYTES_IN_WORDS(x) ((x) * 256)

/* $$$ Replace this stub! */
inline static uint32_t current_tick_count() {
	return 0;
}

inline static void __attribute__((no_instrument_function, always_inline))
					profile_function(block_header_ptr_t header_ptr, uint32_ptr_t timestamp) {
	uint32_t* wptr = (uint32_t*)header_ptr;
	uint32_t idx;

	if (*timestamp > current_tick_count()) {
		return;
	}

	/* CRC the block */
	CRC->CR = CRC_CR_RESET;

	for(idx = 1; idx < header_ptr->block_length; idx++)
	{
		CRC->DR = wptr[idx];
	}

	if (CRC->DR == header_ptr->blockCRC) {
		return;
	}

	flash_copy_to_work(header_ptr, header_ptr->block_length);

	rs_t rs = init_rs(int symsize, int gfpoly, int fcr, int prim, int nroots, int pad)
}


/* ((volatile unsigned long *) 0xE0028000) */



void __attribute__((no_instrument_function, section (".flash0func"))) section0_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash1func"))) section1_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash2func"))) section2_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash3func"))) section3_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash4func"))) section4_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash5func"))) section5_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash6func"))) section6_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash7func"))) section7_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash8func"))) section8_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash9func"))) section9_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash10func"))) section10_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash11func"))) section11_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash12func"))) section12_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash13func"))) section13_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash14func"))) section14_profile_func_enter(void);
void __attribute__((no_instrument_function, section (".flash17func"))) section17_profile_func_enter(void);
/* void __attribute__((no_instrument_function, section (".flash18func"))) section18_profile_func_enter(void); */

#endif /* #define _TRACE_FUNCTIONS_H */