/*
 * Copyright (C) 2016 Nano Avionics
 *
 * Licensed under the GNU GENERAL PUBLIC LICENSE, Version 3 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from the Free Software Foundation, Inc.
 * at
 *
 *    http://fsf.org/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stm32f4xx.h>
#include <trace_functions.h>

const uint32_t FlashSections[FLASH_SECTIONS] = {
    PTR_TO_UINT(FLASH_BASE),	/* This is the work area for fixups */
    PTR_TO_UINT(FLASH_BASE) + K(16),
    PTR_TO_UINT(FLASH_BASE) + K(32),
    PTR_TO_UINT(FLASH_BASE) + K(48),
    PTR_TO_UINT(FLASH_BASE) + K(64),
    PTR_TO_UINT(FLASH_BASE) + K(128),
    PTR_TO_UINT(FLASH_BASE) + K(256),
    PTR_TO_UINT(FLASH_BASE) + K(384),
    PTR_TO_UINT(FLASH_BASE) + K(512),
    PTR_TO_UINT(FLASH_BASE) + K(640),
    PTR_TO_UINT(FLASH_BASE) + K(768),
    PTR_TO_UINT(FLASH_BASE) + K(896)
};

void __cyg_profile_func_enter (void* this_func, void* caller) {
	section1_profile_func_enter((PTR_TO_UINT(caller) - PTR_TO_UINT(BLOCK_BASE)) / sizeof(block_t));
}

/**************************************************************************/

void __cyg_profile_func_exit(void *func, void *caller) {
    return;
}

/**************************************************************************/

static void __attribute__((no_instrument_function)) section1_profile_func_enter(uint32_t block_number) {

	if (crc_check(BLOCK_IN_FLASH1, get_the_time()) != 0)
	{
		/* If this section doesn't pass crc_check(), use the next section to fix this one. */
		section1_profile_func_enter(block_number);
	}

	if (crc_check(block_number, get_the_time()) != 0)
	{
		/* Can't fix a block in the same physical flash section as this function */
		if (data_block_is_in_flash_section(block_number, 1)) {
			section2_profile_func_enter(block_number);
		}
		else {
			fix_block(block_number);
		}
	}
}

/**************************************************************************/

static void __attribute__((no_instrument_function)) section2_profile_func_enter(uint32_t block_number) {

	if (crc_check(BLOCK_IN_FLASH2, get_the_time()) != 0)
	{
		/* If this section doesn't pass crc_check(), use the next section to fix this one. */
		section3_profile_func_enter(BLOCK_IN_FLASH2);
	}

	if (crc_check(block_number, get_the_time()) != 0)
	{
		/* Can't fix a block in the same physical flash section as this function */
		if (data_block_is_in_flash_section(block_number, 2)) {
			section3_profile_func_enter(block_number);
		}
		else {
			fix_block(block_number);
		}
	}
}

/**************************************************************************/

static void __attribute__((no_instrument_function)) section3_profile_func_enter(uint32_t block_number) {

	if (crc_check(BLOCK_IN_FLASH3, get_the_time()) != 0)
	{
		/* If this section doesn't pass crc_check(), use the next section to fix this one. */
		section3_profile_func_enter(BLOCK_IN_FLASH3);
	}

	if (crc_check(block_number, get_the_time()) != 0)
	{
		/* Can't fix a block in the same physical flash section as this function */
		if (data_block_is_in_flash_section(block_number, 3)) {
			/* Silently drift, alone in the cold vacuum of space forever and ever... */
			infinity: goto infinity;
		}
		else {
			fix_block(block_number);
		}
	}
}



