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

const flash_section_t FlashSections[FLASH_SECTIONS] = {
/* The 16K section at 0x08000000 is our work area in case of fixup */
    { ((uint32_t*)FLASH_BASE), SIZE_16K },
    { ((uint32_t*)0x08004000), SIZE_16K },
    { ((uint32_t*)0x08008000), SIZE_16K },
    { ((uint32_t*)0x0800C000), SIZE_16K },
    { ((uint32_t*)0x08010000), SIZE_64K },
    { ((uint32_t*)0x08020000), SIZE_128K },
    { ((uint32_t*)0x08040000), SIZE_128K },
    { ((uint32_t*)0x08060000), SIZE_128K },
    { ((uint32_t*)0x08080000), SIZE_128K },
    { ((uint32_t*)0x080A0000), SIZE_128K }
/* The 128K section at 0x080C0000 is reserved for a work area for the flash copy functions */
/* The 128K section at 0x080E0000 is reserved for .bss and other misc unprotected sections */
 };

const profile_function_t* profile_functions[4] = {
	 (profile_function_t*)section0_profile_func_enter,
	 (profile_function_t*)section1_profile_func_enter,
	 (profile_function_t*)section2_profile_func_enter,
	 (profile_function_t*)section3_profile_func_enter
};

const block_header_t __attribute__((section(".block0hdr"))) block_header0;
const block_header_t __attribute__((section(".block1hdr"))) block_header1;
const block_header_t __attribute__((section(".block2hdr"))) block_header2;
const block_header_t __attribute__((section(".block3hdr"))) block_header3;
const block_header_t __attribute__((section(".block4hdr"))) block_header4;
const block_header_t __attribute__((section(".block5hdr"))) block_header5;
const block_header_t __attribute__((section(".block6hdr"))) block_header6;
const block_header_t __attribute__((section(".block7hdr"))) block_header7;
const block_header_t __attribute__((section(".block8hdr"))) block_header8;
const block_header_t __attribute__((section(".block9hdr"))) block_header9;
const block_header_t __attribute__((section(".block10hdr"))) block_header10;
const block_header_t __attribute__((section(".block11hdr"))) block_header11;
const block_header_t __attribute__((section(".block12hdr"))) block_header12;
const block_header_t __attribute__((section(".block13hdr"))) block_header13;
const block_header_t __attribute__((section(".block14hdr"))) block_header14;
const block_header_t __attribute__((section(".block15hdr"))) block_header15;
const block_header_t __attribute__((section(".block16hdr"))) block_header16;
const block_header_t __attribute__((section(".block17hdr"))) block_header17;
const block_header_t __attribute__((section(".block18hdr"))) block_header18;
const block_header_t __attribute__((section(".block19hdr"))) block_header19;
const block_header_t __attribute__((section(".block20hdr"))) block_header20;
const block_header_t __attribute__((section(".block21hdr"))) block_header21;
const block_header_t __attribute__((section(".block22hdr"))) block_header22;
const block_header_t __attribute__((section(".block23hdr"))) block_header23;
const block_header_t __attribute__((section(".block24hdr"))) block_header24;
const block_header_t __attribute__((section(".block25hdr"))) block_header25;
const block_header_t __attribute__((section(".block26hdr"))) block_header26;
const block_header_t __attribute__((section(".block27hdr"))) block_header27;
const block_header_t __attribute__((section(".block28hdr"))) block_header28;
const block_header_t __attribute__((section(".block29hdr"))) block_header29;
const block_header_t __attribute__((section(".block30hdr"))) block_header30;
const block_header_t __attribute__((section(".block31hdr"))) block_header31;
const block_header_t __attribute__((section(".block32hdr"))) block_header32;
const block_header_t __attribute__((section(".block33hdr"))) block_header33;
const block_header_t __attribute__((section(".block34hdr"))) block_header34;
const block_header_t __attribute__((section(".block35hdr"))) block_header35;
const block_header_t __attribute__((section(".block36hdr"))) block_header36;
const block_header_t __attribute__((section(".block37hdr"))) block_header37;
const block_header_t __attribute__((section(".block38hdr"))) block_header38;
const block_header_t __attribute__((section(".block39hdr"))) block_header39;
const block_header_t __attribute__((section(".block40hdr"))) block_header40;
const block_header_t __attribute__((section(".block41hdr"))) block_header41;
const block_header_t __attribute__((section(".block42hdr"))) block_header42;
const block_header_t __attribute__((section(".block43hdr"))) block_header43;
const block_header_t __attribute__((section(".block44hdr"))) block_header44;
const block_header_t __attribute__((section(".block45hdr"))) block_header45;
const block_header_t __attribute__((section(".block46hdr"))) block_header46;
const block_header_t __attribute__((section(".block47hdr"))) block_header47;
const block_header_t __attribute__((section(".block48hdr"))) block_header48;
const block_header_t __attribute__((section(".block49hdr"))) block_header49;
const block_header_t __attribute__((section(".block50hdr"))) block_header50;
const block_header_t __attribute__((section(".block51hdr"))) block_header51;
const block_header_t __attribute__((section(".block52hdr"))) block_header52;
const block_header_t __attribute__((section(".block53hdr"))) block_header53;
const block_header_t __attribute__((section(".block54hdr"))) block_header54;
const block_header_t __attribute__((section(".block55hdr"))) block_header55;
const block_header_t __attribute__((section(".block56hdr"))) block_header56;
const block_header_t __attribute__((section(".block57hdr"))) block_header57;
const block_header_t __attribute__((section(".block58hdr"))) block_header58;

