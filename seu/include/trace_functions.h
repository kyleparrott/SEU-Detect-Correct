#ifndef _TRACE_FUNCTIONS_H
#define _TRACE_FUNCTIONS_H

#include <reed_solomon.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define PTR_TO_UINT(p) ((uint32_t)(p))
#define UINT_TO_PTR(u) ((void*)(u))

#define CRC_EXPIRE_TIME  42 /* how long between crc checks? */

/* $$$ Replace this stub! */
inline static uint32_t get_the_time() {
	return 0;
}

/***** Flash physical description ***************************/

#define SIZE_16K (1024 * 16)
#define SIZE_64K (1024 * 64)
#define SIZE_128K (1024 * 128)

typedef struct flash_section {
	uint32_t*  base;
	uint32_t size; /* In bytes */
} flash_section_t;

#define FLASH_SECTIONS  (10)
#define FLASH_WORK_AREA UINT_TO_PTR(0x080C0000)
#define WORK_AREA_SIZE	SIZE_128K

/***** Flash logical block description ***************************/
/* NOTE: blockCRC MUST be the first member of this struct! */

typedef struct block_header {
    uint32_t blockCRC;					/*  4 bytes */
	uint16_t reed_solomon_parity[13];   /* 26 bytes */
} block_header_t;

/* These are in bytes (uint8_t) */
#define USEABLE_FLASH_AREA  786432	/* (((uint32_t)&(((uint8_t*)FlashSections[FLASH_SECTIONS - 1].base)[FlashSections[FLASH_SECTIONS - 1].size)) - ((uint32_t)FlashSections[0].base)) */
#define TOTAL_BLOCK_SIZE	13316	/* (sizeof(block_header_t) + DATA_SYMBOL_WORDS * sizeof(DATA_SYMBOL_WORDS)) */
#define BLOCK_COUNT 		59		/* (USEABLE_FLASH_AREA / TOTAL_BLOCK_SIZE) */
#define BLOCK_BASE			((block_header_t*)(FlashSections[0].base))

/***** Profiler functions ***************************/

#define PROFILE_FUNCTION_COUNT 4
#define PROFILE_FUNCTION_MASK  ((1 << PROFILE_FUNCTION_COUNT) - 1)
typedef void (*profile_function_t)(uint32_t block_number);

inline static uint32_t __attribute__((no_instrument_function, always_inline)) crc_block(uint32_t* wptr) {
	register uint32_t* endptr;
	register uint32_t crc_value;

	CRC->CR = CRC_CR_RESET;
	endptr = (uint32_t*)&((block_header_t*)wptr)[1];
	crc_value = *wptr++;
	while(wptr < endptr)
	{
		CRC->DR = *wptr++;
	}

	return (CRC->DR == crc_value ? 1 : 0);
}

inline static void __attribute__((no_instrument_function, always_inline)) profile_function(uint32_t block_number, uint32_t tm_now) {

	if (crc_expire_time[block_number] > tm_now) {
		return;
	}

	crc_expire_time[block_number] = tm_now + CRC_EXPIRE_TIME;

	if (crc_block(block_number)) {
		return;
	}


	uint32_t* wptr = (uint32_t*)&BLOCK_BASE[block_number];
/*	flash_copy_to_work(header_ptr, header_ptr->block_length);

	rs_t rs = init_rs(int symsize, int gfpoly, int fcr, int prim, int nroots, int pad)
 */
}

void __attribute__((no_instrument_function, section (".block0func"))) section0_profile_func_enter(uint32_t block_number);
void __attribute__((no_instrument_function, section (".block1func"))) section1_profile_func_enter(uint32_t block_number);
void __attribute__((no_instrument_function, section (".block2func"))) section2_profile_func_enter(uint32_t block_number);
void __attribute__((no_instrument_function, section (".block3func"))) section3_profile_func_enter(uint32_t block_number);

#endif /* #define _TRACE_FUNCTIONS_H */
