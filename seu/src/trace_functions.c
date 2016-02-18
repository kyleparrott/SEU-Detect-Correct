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
 
// #include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h"
#include "block_header.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_rcc.h"

GPIO_InitTypeDef  GPIO_InitStructure;
void Delay(__IO uint32_t nCount);

void __cyg_profile_func_enter (void *func, void *caller) {
    // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    // GPIO_SetBits(GPIOD, GPIO_Pin_12);

    // Delay(0xFFFFF);
    // CRC_ResetDR();
    // CRC->CR=1;

    // uint32_t addr = (uint32_t)func;
    // blockHeader* pointer = (blockHeader*)(addr-9);  //-9  (hardcoded for my testing so far) because struct size = 8 and a 1 byte arm thumb code pointer between
    //                                                 //the header and the function.

    // uint32_t* functionPtr = (uint32_t*)func;
    // uint32_t calcCrc = CRC_CalcBlockCRC(functionPtr, pointer->length/4);
    // CRC_ResetDR();

    // if(pointer->blockCRC == calcCrc) {
    //     GPIO_SetBits(GPIOD, GPIO_Pin_15);
    // } else {
    //     GPIO_SetBits(GPIOD, GPIO_Pin_14);
    // }
    // Delay(0xFFFFF);
    // GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
    // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, DISABLE);
    // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, DISABLE);
}

void __cyg_profile_func_exit(void *func, void *caller) {
    return;
}

void Delay(__IO uint32_t nCount) {
  while(nCount--) {
  }
}
