

#include <stdint.h>

#define FLASH_CR_SNB (FLASH_CR_SNB_0 | FLASH_CR_SNB_1 | FLASH_CR_SNB_2 | FLASH_CR_SNB_3)
#define FLASH_CR_PSIZE (FLASH_CR_PSIZE_0 | FLASH_CR_PSIZE_1)
#define ALL_BITS (~0u)

#define set_reg_bits(r, bitpat, mask) r = ((r) & (~mask) | ((bitpat) & (mask))
#define clear_reg_bits(r, mask) r = ((r) & (~mask))
#define test_reg_bits(r, mask) ((r) & (~mask))

#define PSIZE_BYTE  0u
#define PSIZE_WORD  1u
#define PSIZE_DWORD 2u

typedef struct {
	uint32_t address;
	uint32_t size;
} flash_section_t;

#define FLASH_SECTION_COUNT 12

flash_section_t flash_sections[FLASH_SECTION_COUNT] = {
	{ 0x08000000u, 0x00004000u },
	{ 0x08004000u, 0x00004000u },
	{ 0x08008000u, 0x00004000u },
	{ 0x0800C000u, 0x00004000u },
	{ 0x08010000u, 0x00010000u },
	{ 0x08020000u, 0x00020000u },
	{ 0x08040000u, 0x00020000u },
	{ 0x08060000u, 0x00020000u },
	{ 0x08080000u, 0x00020000u },
	{ 0x080A0000u, 0x00020000u },
	{ 0x080C0000u, 0x00020000u },
	{ 0x080E0000u, 0x00020000u }
};

void fix_flash(UBaseType_t *ptr1, UBaseType_t corrected_value1,
			   UBaseType_t *ptr2, UBaseType_t corrected_value2,
			   UBaseType_t *ptr3, UBaseType_t corrected_value3);

void copy_dword_block(UBaseType_t segnum, ptr, UBaseType_t corrected_value)
void copy_word_block(UBaseType_t segnum, ptr, UBaseType_t corrected_value)
void copy_byte_block(UBaseType_t segnum, ptr, UBaseType_t corrected_value)
