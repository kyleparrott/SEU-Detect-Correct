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

block_header_t __attribute__((section(".flash0hdr"))) FlashHeader0 = { 0, BL_16K, {0}};
block_header_t __attribute__((section(".flash1hdr"))) FlashHeader1 = { 0, BL_16K, {0}};
block_header_t __attribute__((section(".flash2hdr"))) FlashHeader2 = { 0, BL_16K, {0}};
block_header_t __attribute__((section(".flash3hdr"))) FlashHeader3 = { 0, BL_16K, {0}};
block_header_t __attribute__((section(".flash4hdr"))) FlashHeader4 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash5hdr"))) FlashHeader5 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash6hdr"))) FlashHeader6 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash7hdr"))) FlashHeader7 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash8hdr"))) FlashHeader8 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash9hdr"))) FlashHeader9 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash10hdr"))) FlashHeader10 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash11hdr"))) FlashHeader11 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash12hdr"))) FlashHeader12 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash13hdr"))) FlashHeader13 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash14hdr"))) FlashHeader14 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash17hdr"))) FlashHeader17 = { 0, BL_64K, {0}};
block_header_t __attribute__((section(".flash18hdr"))) FlashHeader18 = { 0, BL_64K, {0}};

block_header_t* block_pointers[18] = {
	(block_header_t*)&FlashHeader0,
	(block_header_t*)&FlashHeader1,
	(block_header_t*)&FlashHeader2,
	(block_header_t*)&FlashHeader3,
	(block_header_t*)&FlashHeader4,
	(block_header_t*)&FlashHeader5,
	(block_header_t*)&FlashHeader6,
	(block_header_t*)&FlashHeader7,
	(block_header_t*)&FlashHeader8,
	(block_header_t*)&FlashHeader9,
	(block_header_t*)&FlashHeader10,
	(block_header_t*)&FlashHeader11,
	(block_header_t*)&FlashHeader12,
	(block_header_t*)&FlashHeader13,
	(block_header_t*)&FlashHeader14,
	NULL,
	NULL,
	(block_header_t*)&FlashHeader17
};

profile_function_t* section_pointers[18] = {
	 (profile_function_t*)section0_profile_func_enter,
	 (profile_function_t*)section1_profile_func_enter,
	 (profile_function_t*)section2_profile_func_enter,
	 (profile_function_t*)section3_profile_func_enter,
	 (profile_function_t*)section4_profile_func_enter,
	 (profile_function_t*)section5_profile_func_enter,
	 (profile_function_t*)section6_profile_func_enter,
	 (profile_function_t*)section7_profile_func_enter,
	 (profile_function_t*)section8_profile_func_enter,
	 (profile_function_t*)section9_profile_func_enter,
	 (profile_function_t*)section10_profile_func_enter,
	 (profile_function_t*)section11_profile_func_enter,
	 (profile_function_t*)section12_profile_func_enter,
	 (profile_function_t*)section13_profile_func_enter,
	 (profile_function_t*)section14_profile_func_enter,
	 NULL,
	 NULL,
	 (profile_function_t*)section17_profile_func_enter
};

uint32_t timestamps[16];

profile_function_t* LastBlockChecked = NULL;

void __cyg_profile_func_enter (void* this_func, void* caller) {
	int index = (int)((((uint32_t)caller) >> 16) & 0x0000000F);
	if (index == 0) {
		index = (int)((((uint32_t)caller) >> 14) & 0x00000003);
	}

	if (LastBlockChecked == NULL) {
		LastBlockChecked = section_pointers[index];
	}

	(*LastBlockChecked)(block_pointers[index], &timestamps[index]);

	LastBlockChecked = section_pointers[index];
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash0func"))) section0_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash1func"))) section1_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash2func"))) section2_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash3func"))) section3_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash4func"))) section4_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash5func"))) section5_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash6func"))) section6_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash7func"))) section7_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash8func"))) section8_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash9func"))) section9_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash10func"))) section10_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash11func"))) section11_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash12func"))) section12_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash13func"))) section13_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash14func"))) section14_profile_func_enter(block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/
/*
 * Sections 15 and 16 are used as a work area so no CRC.
 */
/**************************************************************************/

void __attribute__((no_instrument_function, section (".flash17func"))) section17_profile_func_enter (block_header_t* blk_ptr, uint32_t* timestamp) {
	profile_function(blk_ptr, timestamp);
}

/**************************************************************************/
/*
void section18_profile_func_enter (void) {
	profile_function(blk_ptr, timestamp);
}
*/
void __cyg_profile_func_exit(void *func, void *caller) {
    return;
}
