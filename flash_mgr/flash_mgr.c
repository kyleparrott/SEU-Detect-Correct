#include "flash_mgr.h"


/* Temp so I can compile */
int main()
{
	return 0;
}


void fix_flash(uint16_t **ptrs, uint16_t *corrected_values)
{
	uint32_t psize;
	uint_t   segnum = get_flash_segment_number(ptr1);

/* Unlock the FLASH_CR register */
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;

/* $$$ Check voltage */

	psize = PSIZE_DWORD; /* 0x02u	DWord (uint32_t) 2.7 - 3.6v
	                         0x01u	Word (uint16_t) 2.1 - 2.7v
	                         0x00u	Byte (uint8_t)  1.8 - 2.1v */

/* Set the access size */
	set_reg_bits(FLASH->CR, psize, FLASH_CR_PSIZE);

/* Make sure the flash isn't already doing something */
	while (test_reg_bits(FLASH->SR, FLASH_SR_BSY)) {
		/* wait for it to get done */
	}

	segnum = get_flash_segment_number(ptr);

/* $$$ copy the section elsewhere */

/* erase the section */
	set_reg_bits(FLASH->CR, ALL_BITS, FLASH_CR_SER);
	set_reg_bits(FLASH->CR, (segnum << SNB_SHIFT), FLASH_CR_SNB);
	set_reg_bits(FLASH->CR, ALL_BITS, FLASH_CR_STRT);
	while (test_reg_bits(FLASH->SR, FLASH_SR_BSY)) {
		/* wait for it to get done */
	}

	switch (psize) {
		case PSIZE_DWORD:
			copy_dword_block(segnum, ptr, corrected_value);
			break;

		case PSIZE_WORD:
			copy_word_block(segnum, ptr, corrected_value);
			break;

		default: /* PSIZE_BYTE */
			copy_byte_block(segnum, ptr, corrected_value);
	}

	copy_dword_block(segnum, ptr, corrected_value)
	copy_word_block(segnum, ptr, corrected_value)
	copy_byte_block(segnum, ptr, corrected_value)

	while (/* more data to copy */) {
	set_reg_bits(FLASH->CR, ~0u, FLASH_CR_PR);

	/* $$$ copy the section back EXCEPT *ptr */
	/* copy back based on psize */

	*ptr = corrected_value;
