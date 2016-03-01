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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "crcmodel.h"
#include "crc_generator.h"
#include "reed_solomon.h"

void encode_and_crc (char* input);
static uint32_t CRC_CalcBlockCRC (uint32_t *buffer, uint32_t words);

int main(int argc, char** argv) {
    if (argc == 1) {  //If no input is given, it will create some sample data, encode and crc it to show it working.
                      //This is just a temporary measure. I will remove this whole block once we know it's working properly.
        printf("Running with duplicate random data set:\n");
        srandom(time(NULL));
        word_t block[SYMBOL_TABLE_WORDS];
        int i;
        for (i = 0; i < SYMBOL_TABLE_WORDS; i++) {
            block[i] = random();
        }

        block_t testBlocks[2];
        memcpy(&(testBlocks[0]), block, sizeof(block));
        memcpy(&(testBlocks[1]), block, sizeof(block));
        testBlocks[0].crc = 0;
        testBlocks[1].crc = 0;

        struct blockWrapper* testBlockWrapper = malloc(sizeof(struct blockWrapper));
        testBlockWrapper->block_count = 2;
        testBlockWrapper->data = malloc(sizeof(block_t) * 2);

        memcpy((testBlockWrapper->data), testBlocks, sizeof(testBlocks));
        encode_and_crc((char*) testBlockWrapper);
    } else {
        encode_and_crc(argv[1]);
    }
    return 0;
}

void encode_and_crc (char* input) {
    struct blockWrapper* blockWrapperPtr = (struct blockWrapper*) input;
    uint32_t inputData [SYMBOL_TABLE_WORDS/2]; //SYMBOL_TABLE_WORDS in 16 bit words.
    uint32_t dataSize = sizeof(block_t) - sizeof(uint32_t);
    uint32_t numWords = dataSize / sizeof(uint32_t);
    uint32_t crc;
    
    uint16_t parityData[PARITY_SYMBOL_COUNT]; //used to print hex of parity symbols

    int idx, x;
    int parityStartIdx = SYMBOL_TABLE_WORDS - PARITY_SYMBOL_COUNT - 1;

    for (idx = 0; idx < blockWrapperPtr->block_count; idx++) {
        memcpy(inputData, &(blockWrapperPtr->data[idx]), dataSize);
        encode_rs((word_t*) inputData);
        crc = CRC_CalcBlockCRC((uint32_t*)inputData, numWords);
        blockWrapperPtr->data[idx].crc = crc;

        memcpy(parityData, &(blockWrapperPtr->data[idx].reed_solomon_data[parityStartIdx]), PARITY_SYMBOL_COUNT);
        printf("%u,", crc);
        for (x = 0; x < PARITY_SYMBOL_COUNT; x++) {
            printf("%x", parityData[x]);
        }
        printf("\n");
    }
}

static uint32_t CRC_CalcBlockCRC(uint32_t *buffer, uint32_t words) {
     cm_t           crc_model;
     uint32_t       word_to_do;
     unsigned char  byte_to_do;
     int            i;

     // Values for the STM32F generator.

     crc_model.cm_width = 32;            // 32-bit CRC
     crc_model.cm_poly  = 0x04C11DB7;    // CRC-32 polynomial
     crc_model.cm_init  = 0xFFFFFFFF;    // CRC initialized to 1's
     crc_model.cm_refin = FALSE;         // CRC calculated MSB first
     crc_model.cm_refot = FALSE;         // Final result is not bit-reversed
     crc_model.cm_xorot = 0x00000000;    // Final result XOR'ed with this

     cm_ini(&crc_model);

     while (words--)
     {
         // The STM32F10x hardware does 32-bit words at a time!!!

         word_to_do = *buffer++;

         // Do all bytes in the 32-bit word.

         for (i = 0; i < sizeof(word_to_do); i++)
         {
             // We calculate a *byte* at a time. If the CRC is MSB first we
             // do the next MS byte and vica-versa.

             if (crc_model.cm_refin == FALSE)
             {
                 // MSB first. Do the next MS byte.

                 byte_to_do = (unsigned char) ((word_to_do & 0xFF000000) >> 24);
                 word_to_do <<= 8;
             }
             else
             {
                 // LSB first. Do the next LS byte.

                 byte_to_do = (unsigned char) (word_to_do & 0x000000FF);
                 word_to_do >>= 8;
             }

             cm_nxt(&crc_model, byte_to_do);
         }
     }

     // Return the final result.

     return (cm_crc(&crc_model));
 }
