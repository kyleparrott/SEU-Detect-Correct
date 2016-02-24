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
#include <stm32f4xx_flash.h>
#include "crc.h"

const uint32_t BLK_15_START = 0x080C0000;
const uint8_t WORK_FLASH_SECTOR = 10; //Just using sector 10 as the temporary work area, Sector11 still hold program data.


block_header_t __attribute__((section(".flash00hdr"))) FlashHeader0;
block_header_t __attribute__((section(".flash01hdr"))) FlashHeader1;
block_header_t __attribute__((section(".flash02hdr"))) FlashHeader2;
block_header_t __attribute__((section(".flash03hdr"))) FlashHeader3;
block_header_t __attribute__((section(".flash04hdr"))) FlashHeader4;
block_header_t __attribute__((section(".flash05hdr"))) FlashHeader5;
block_header_t __attribute__((section(".flash06hdr"))) FlashHeader6;
block_header_t __attribute__((section(".flash07hdr"))) FlashHeader7;
block_header_t __attribute__((section(".flash08hdr"))) FlashHeader8;
block_header_t __attribute__((section(".flash09hdr"))) FlashHeader9;
block_header_t __attribute__((section(".flash10hdr"))) FlashHeader10;
block_header_t __attribute__((section(".flash11hdr"))) FlashHeader11;
block_header_t __attribute__((section(".flash12hdr"))) FlashHeader12;
block_header_t __attribute__((section(".flash13hdr"))) FlashHeader13;
block_header_t __attribute__((section(".flash14hdr"))) FlashHeader14;
block_header_t __attribute__((section(".flash15hdr"))) FlashHeader15;
block_header_t __attribute__((section(".flash16hdr"))) FlashHeader16;
block_header_t __attribute__((section(".flash17hdr"))) FlashHeader17;

/* block_header_t FlashHeader18 __attribute__((section("flash18"))); */

CRCCheck_t sections[15] = {
	 NULL,
	 section04_profile_func_enter,
	 section05_profile_func_enter,
	 section06_profile_func_enter,
	 section07_profile_func_enter,
	 section08_profile_func_enter,
	 section09_profile_func_enter,
	 section10_profile_func_enter,
	 section11_profile_func_enter,
	 section12_profile_func_enter,
	 section13_profile_func_enter,
	 section14_profile_func_enter,
	 section15_profile_func_enter,
	 section16_profile_func_enter,
	 section17_profile_func_enter /* ,
	 section18_profile_func_enter */
};

CRCCheck_t little_sections[4] = {
	 section00_profile_func_enter,
	 section01_profile_func_enter,
	 section02_profile_func_enter,
	 section03_profile_func_enter
};



void __cyg_profile_func_enter (void* this_func, void* caller)
			{

	uint32_t secnum = (((uint32_t)caller) >> 16) & 0x0000000F;

	if (secnum > 0) {
		(*sections[secnum])();
	}
	else {
		secnum = (((uint32_t)caller) >> 14) & 3u;
		(*sections[secnum])();
	}
}

/**************************************************************************/

void section00_profile_func_enter (void) {
	profile_function(&FlashHeader0);
}

/**************************************************************************/

void section01_profile_func_enter (void) {
	profile_function(&FlashHeader1);
}

/**************************************************************************/

void section02_profile_func_enter (void) {
	profile_function(&FlashHeader2);
}

/**************************************************************************/

void section03_profile_func_enter (void) {
	profile_function(&FlashHeader3);
}

/**************************************************************************/

void section04_profile_func_enter (void) {
	profile_function(&FlashHeader4);
}

/**************************************************************************/

void section05_profile_func_enter (void) {
	profile_function(&FlashHeader5);
}

/**************************************************************************/

void section06_profile_func_enter (void) {
	profile_function(&FlashHeader6);
}

/**************************************************************************/

void section07_profile_func_enter (void) {
	profile_function(&FlashHeader7);
}

/**************************************************************************/

void section08_profile_func_enter (void) {
	profile_function(&FlashHeader8);
}

/**************************************************************************/

void section09_profile_func_enter (void) {
	profile_function(&FlashHeader9);
}

/**************************************************************************/

void section10_profile_func_enter (void) {
	profile_function(&FlashHeader10);
}

/**************************************************************************/

void section11_profile_func_enter (void) {
	profile_function(&FlashHeader11);
}

/**************************************************************************/

void section12_profile_func_enter (void) {
	profile_function(&FlashHeader12);
}

/**************************************************************************/

void section13_profile_func_enter (void) {
	profile_function(&FlashHeader13);
}

/**************************************************************************/

void section14_profile_func_enter (void) {
	profile_function(&FlashHeader14);
}

/**************************************************************************/

void section15_profile_func_enter (void) {
	profile_function(&FlashHeader15);
}

/**************************************************************************/

void section16_profile_func_enter (void) {
	profile_function(&FlashHeader16);
}

/**************************************************************************/

void section17_profile_func_enter (void) {
	profile_function(&FlashHeader17);
}

/**************************************************************************/
/*
void section18_profile_func_enter (void) {
	profile_function(&FlashHeader18);
}
*/
void __cyg_profile_func_exit(void *func, void *caller) {
    return;
}

void flash_copy_to_work(uint32_t* blk_ptr, uint32_t blk_size){
	FLASH_Unlock();
	FLASH_SetLatency(FLASH_Latency_5); //My discovery board running @ 168 mhz, 2.907v
	FLASH_EraseSector(WORK_FLASH_SECTOR, VoltageRange_3);
	uint32_t idx;
	uint32_t dest = BLK_15_START;
	for (idx = 0; idx < blk_size; idx++) {
		FLASH_ProgramWord(dest + idx, *blk_ptr + idx);
	}
	FLASH_Lock();
}

 void flash_copy_from_work(uint32_t* blk_ptr, uint32_t blk_size, uint32_t* fix_ptrs[16], uint32_t fix_values[16], uint32_t fix_count) {
 	uint32_t* src_blk = (uint32_t*) BLK_15_START;
 	uint32_t* new_address = 0;
 	uint8_t  fixup_index = 0; // keeps track of fix_ptrs, fix_values iterations

 	FLASH_Unlock();
	FLASH_SetLatency(FLASH_Latency_5); //My discovery board running @ 168 mhz, 2.907v
	FLASH_EraseSector(WORK_FLASH_SECTOR, VoltageRange_3);

 	uint32_t  idx;
 	for (idx = 0; idx < blk_size; idx++) {
 		new_address = blk_ptr + idx; 

 		if (new_address == fix_ptrs[fixup_index]) { // this assumes the fix_ptrs[] elements are sorted
 			FLASH_ProgramWord((uint32_t) new_address, fix_values[fixup_index]);
 			fixup_index++;
 		} else {
 			FLASH_ProgramWord((uint32_t) new_address, src_blk[idx]);
 		}
 	}
    FLASH_Lock();
 }















