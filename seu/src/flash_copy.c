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

#include <stm32f4xx_flash.h>

const uint32_t BLK_15_START = 0x080C0000;
const uint8_t WORK_FLASH_SECTOR = 10; //Just using sector 10 as the temporary work area, Sector11 still hold program data.


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

 void flash_copy_from_work(uint32_t* blk_ptr, uint32_t blk_size, uint32_t* fix_ptrs[8], uint32_t fix_values[8], uint32_t fix_count) {
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







