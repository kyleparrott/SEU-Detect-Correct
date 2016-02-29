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

uint32_t crc_expire_times[BLOCK_COUNT];

const flash_section_t FlashSections[FLASH_SECTIONS] = {
/* The 16K section at 0x08000000 is our work area in case of fixup */
    { ((uint32_t)FLASH_BASE), SIZE_16K },
    { ((uint32_t)0x08004000), SIZE_16K },
    { ((uint32_t)0x08008000), SIZE_16K },
    { ((uint32_t)0x0800C000), SIZE_16K },
    { ((uint32_t)0x08010000), SIZE_64K },
    { ((uint32_t)0x08020000), SIZE_128K },
    { ((uint32_t)0x08040000), SIZE_128K },
    { ((uint32_t)0x08060000), SIZE_128K },
    { ((uint32_t)0x08080000), SIZE_128K },
    { ((uint32_t)0x080A0000), SIZE_128K }
/* The 128K section at 0x080C0000 is reserved for a work area for the flash copy functions */
/* The 128K section at 0x080E0000 is reserved for .bss and other misc unprotected sections */
 };

const profile_function_t profile_functions[4] = {
	(profile_function_t) section0_profile_func_enter,
	(profile_function_t) section1_profile_func_enter,
	(profile_function_t) section2_profile_func_enter,
	(profile_function_t) section3_profile_func_enter
};

void __cyg_profile_func_enter (void* this_func, void* caller) {
	register uint32_t block_number = (PTR_TO_UINT(caller) - BLOCK_BASE) / TOTAL_BLOCK_SIZE;

	profile_functions[block_number & PROFILE_FUNCTION_MASK](block_number);
}

/**************************************************************************/

void __cyg_profile_func_exit(void *func, void *caller) {
    return;
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block0func"))) section0_profile_func_enter(uint32_t block_number) {

	if (crc_check(0, get_the_time()) != 0)
	{
		/* If this section doesn't pass (), use the next section to fix this one. */
		section1_profile_func_enter(0);
	}

	if (crc_check(block_number, get_the_time()) != 0)
	{
		fix_block(block_number);
	}
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block1func"))) section1_profile_func_enter(uint32_t block_number) {

	if (crc_check(1, get_the_time()) != 0)
	{
		section2_profile_func_enter(1);
	}

	if (crc_check(block_number, get_the_time()) != 0)
	{
		fix_block(block_number);
	}
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block2func"))) section2_profile_func_enter(uint32_t block_number) {

	if (crc_check(2, get_the_time()) != 0)
	{
		section3_profile_func_enter(2);
	}

	if (crc_check(block_number, get_the_time()) != 0)
	{
		fix_block(block_number);
	}
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block3func"))) section3_profile_func_enter(uint32_t block_number) {

	if (crc_check(3, get_the_time()) != 0)
	{
		section0_profile_func_enter(3);
	}

	if (crc_check(block_number, get_the_time()) != 0)
	{
		fix_block(block_number);
	}
}



