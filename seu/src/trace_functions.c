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
#include "crc.h"

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
