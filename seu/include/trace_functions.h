#ifndef _TRACE_FUNCTIONS_H
#define _TRACE_FUNCTIONS_H

#include <reed_solomon.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define PTR_TO_UINT(p) ((uint32_t)(p))
#define UINT_TO_PTR(u) ((void*)(u))

#define CRC_EXPIRE_TIME  42 /* how long between crc checks? */
extern uint32_t crc_expire_times[BLOCK_COUNT];

/* $$$ Replace this stub! */
inline static uint32_t get_the_time() {
	return 0;
}

/***** Flash physical description ***************************/

#define SIZE_16K (1024 * 16)
#define SIZE_64K (1024 * 64)
#define SIZE_128K (1024 * 128)

typedef struct flash_section {
	uint32_t*	base;
	uint32_t	size; /* In bytes */
} flash_section_t;

#define FLASH_SECTIONS  (10)
#define FLASH_BASE		(0x08000000)
#define FLASH_END		(0x080C0000)
#define FLASH_WORK_AREA UINT_TO_PTR(FLASH_END)
#define WORK_AREA_SIZE	SIZE_128K
#ifndef CONSTANTS_C
extern const flash_section_t FlashSections[FLASH_SECTIONS];
#endif

/* These are in bytes (uint8_t) */
#define USEABLE_FLASH_AREA  	(FLASH_END - FLASH_BASE)
#define BLOCK_USABLE_CODE_SIZE 	PAYLOAD_DATA_BYTES
#define REED_SOLOMON_BLOCK_SIZE (SYMBOL_TABLE_WORDS * sizeof(word_t))
#define CRC_SIZE				(sizeof(uint32_t))
#define TOTAL_BLOCK_SIZE		(REED_SOLOMON_BLOCK_SIZE + SIZEOF_CRC)
#define BLOCK_COUNT 			(USEABLE_FLASH_AREA / TOTAL_BLOCK_SIZE)
#define BLOCK_BASE				(FLASH_BASE)
#define BLOCK_START(b)			(BLOCK_BASE + (b) * TOTAL_BLOCK_SIZE)


#define PROFILE_FUNCTION_COUNT 4
#define PROFILE_FUNCTION_MASK  ((1 << PROFILE_FUNCTION_COUNT) - 1)
typedef void (*profile_function_t)(uint32_t block_number);
#ifndef CONSTANTS_C
const profile_function_t* profile_functions[PROFILE_FUNCTION_COUNT]
#endif


/***** CRC functions ***************************/

inline static uint32_t INLINE_ATTRIBUTE crc_check(uint32_t block_number, uint32_t tm_now) {
	register uint32_t* ptr;
	register uint32_t* crc_ptr;

	if (crc_expire_times[block_number] > tm_now) {
		return 0;
	}

	CRC->CR = CRC_CR_RESET;

	ptr = UINT_TO_PTR(BLOCK_START(block_number));
	crc_ptr = UINT_TO_PTR((PTR_TO_UINT(ptr) + TOTAL_BLOCK_SIZE - CRC_SIZE));

	while(ptr < crc_ptr)
	{
		CRC->DR = *ptr++;
	}

	if (CRC->DR ^ *crc_ptr) {
		return 1;
	}

	crc_expire_times[block_number] = tm_now + CRC_EXPIRE_TIME;

	return 0;
}

/***** Profiler functions ***************************/

inline static int INLINE_ATTRIBUTE profile_function(uint32_t block_number, uint32_t tm_now) {


	uint32_t* this = (uint32_t*)&((unit8_t)BLOCK_BASE)[block_number * TOTAL_BLOCK_SIZE];

	if (crc_block(this, block_number) < 0) {
		return -1; /* This block contains an error so use the next block to fix this one. */
	}

	crc_expire_times[block_number] = tm_now + CRC_EXPIRE_TIME;



/*	flash_copy_to_work(header_ptr, header_ptr->block_length);

	rs_t rs = init_rs(int symsize, int gfpoly, int fcr, int prim, int nroots, int pad)
 */
}

void __attribute__((no_instrument_function, section (".block0func"))) section0_profile_func_enter(uint32_t block_number);
void __attribute__((no_instrument_function, section (".block1func"))) section1_profile_func_enter(uint32_t block_number);
void __attribute__((no_instrument_function, section (".block2func"))) section2_profile_func_enter(uint32_t block_number);
void __attribute__((no_instrument_function, section (".block3func"))) section3_profile_func_enter(uint32_t block_number);

#endif /* #define _TRACE_FUNCTIONS_H */
