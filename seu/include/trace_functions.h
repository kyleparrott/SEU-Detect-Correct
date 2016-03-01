#ifndef _TRACE_FUNCTIONS_H
#define _TRACE_FUNCTIONS_H

#include <reed_solomon.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define PTR_TO_UINT(p) ((uint32_t)(p))
#define UINT_TO_PTR(u) ((void*)(u))

/***** Flash physical description ***************************/

#define K(x) (1024 * (x))
#define FLASH_SECTIONS  (12)
#define WORK_SECTION	(0)

extern const uint32_t FlashSections[FLASH_SECTIONS];

typedef struct block {
	uint16_t	reed_solomon_data[SYMBOL_TABLE_WORDS]; /*same as 3328 * sizeof(uint32_t) so the alignment works. */
	uint32_t	crc;
} block_t;

#define BLOCK_IN_FLASH1	0
#define BLOCK_IN_FLASH2	3
#define BLOCK_IN_FLASH3	4

/* The linker will assign these values... */
extern uint32_t work_area;
extern uint32_t block_count;
extern block_t  block_base;
extern uint32_t crc_expire_times;

#define WORK_AREA		(&work_area)
#define BLOCK_COUNT 	(*(&block_count))
#define BLOCK_BASE		(&block_base)
#define BLOCK_START(b)	(&BLOCK_BASE[(b)])

#define CRC_EXPIRE_TIME  42 /* how long between crc checks? */

/* $$$ Replace this stub! */
inline static uint32_t get_the_time() {
	return 0;
}

static void __attribute__((no_instrument_function)) section1_profile_func_enter(uint32_t block_number);
static void __attribute__((no_instrument_function)) section2_profile_func_enter(uint32_t block_number);
static void __attribute__((no_instrument_function)) section3_profile_func_enter(uint32_t block_number);


/***** Test if block[block_number] overlaps flash_section[section_number] */

inline static uint32_t INLINE_ATTRIBUTE data_block_is_in_flash_section(uint32_t block_number, uint32_t section_number) {

	if (PTR_TO_UINT(BLOCK_START(block_number)) >= FlashSections[section_number + 1]) {
		return 0;
	}

	if (PTR_TO_UINT(BLOCK_START(block_number + 1)) <= FlashSections[section_number]) {
		return 0;
	}

	return 1;
}

/***** CRC function ***************************/

inline static uint32_t INLINE_ATTRIBUTE crc_check(uint32_t block_number, uint32_t tm_now) {
	register uint32_t* ptr;
	register uint32_t* crc_ptr;

	if ((&crc_expire_times)[block_number] > tm_now) {
		return 0;
	}

	CRC->CR = CRC_CR_RESET;

	ptr = UINT_TO_PTR(BLOCK_START(block_number));
	crc_ptr = UINT_TO_PTR(BLOCK_START(block_number + 1) - sizeof(uint32_t));

	while(ptr < crc_ptr)
	{
		CRC->DR = *ptr++;
	}

	if (CRC->DR ^ *crc_ptr) {
		return 1;
	}

	(&crc_expire_times)[block_number] = tm_now + CRC_EXPIRE_TIME;

	return 0;
}

/***** block fix functions ***************************/

inline static void INLINE_ATTRIBUTE fix_block(uint32_t block_number) {
	word_t error_offsets[RS_MAX_ERRORS];
	word_t corrected_values[RS_MAX_ERRORS];

	uint32_t blk_start = PTR_TO_UINT(BLOCK_START(block_number));
	uint32_t error_location;

	int error_count = decode_rs(UINT_TO_PTR(blk_start), error_offsets, corrected_values);
	int i, j;

	for(i = 0; i < error_count; i++) {
		error_location = blk_start + error_offsets[i];
		for(j=1; j < FLASH_SECTIONS; j++) {
			if (error_location >= FlashSections[j] &&
			    error_location < FlashSections[j + 1]) {
/* $$$ Call Copy Flash functions here */
			}
		}

	}
}

#endif /* #define _TRACE_FUNCTIONS_H */
