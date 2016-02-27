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

uint32_t crc_expire_time[BLOCK_COUNT];

void __cyg_profile_func_enter (void* this_func, void* caller) {
	register uint32_t block_number = (PTR_TO_UINT(caller) - BLOCK_BASE) / TOTAL_BLOCK_SIZE;

	*profile_functions[block_number & 0x00000003](block_number);
}

/**************************************************************************/

void __cyg_profile_func_exit(void *func, void *caller) {
    return;
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block0func"))) section0_profile_func_enter(uint32_t block_number) {
	profile_function0(0);
	profile_function0(block_number);
}

void __attribute__((no_instrument_function, section (".block0func"))) profile_function0(uint32_t block_number) {
	profile_function(block_number);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block1func"))) section1_profile_func_enter(uint32_t block_number) {
	profile_function1(1);
	profile_function1(block_number);
}

void __attribute__((no_instrument_function, section (".block1func"))) profile_function1(uint32_t block_number) {
	profile_function(block_number);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block2func"))) section2_profile_func_enter(uint32_t block_number) {
	profile_function2(2);
	profile_function2(block_number);
}

void __attribute__((no_instrument_function, section (".block2func"))) profile_function2(uint32_t block_number) {
	profile_function(block_number);
}

/**************************************************************************/

void __attribute__((no_instrument_function, section (".block3func"))) section3_profile_func_enter(uint32_t block_number) {
	profile_function3(3);
	profile_function3(block_number);
}

void __attribute__((no_instrument_function, section (".block3func"))) profile_function3(uint32_t block_number) {
	profile_function(block_number);
}



