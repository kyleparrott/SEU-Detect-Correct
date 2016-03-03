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

#define SIXTEEN_K (1024 * 16) //Linkerscript leaves 16k space before block_count symbol

static uint32_t CRC_CalcBlockCRC (uint32_t *buffer, uint32_t words);

int main(int argc, char** argv) { 
    // arguments: argv[1] input file name. argv[2] offset to start of .text section
    char* outputFileName = "binary/encodedFinalFreeRTOS.elf\0"; // hardcoded output name
    
    FILE* outputFile; 
    FILE* inputFile;
    
    if ((inputFile = fopen(argv[1], "rb")) == NULL) {
        printf("Error opening %s\n", argv[1]);
        return 1;
    }

    if ((outputFile = fopen(outputFileName, "wb")) == NULL) {
        printf("Error opening %s\n", outputFileName);
        return 1;
    }

    int inputFileLen;
    char* inputData;

    if (fseek(inputFile, 0L, SEEK_END) == 0) { //Gets length of file and reads input file in to memory
        inputFileLen = ftell(inputFile);
        inputData = malloc(inputFileLen);
        fseek(inputFile, 0L, SEEK_SET);
        fread(inputData, sizeof(char), inputFileLen, inputFile);
    }

    uint32_t offset = strtol(argv[2], NULL, 16);
    long* blockCount = (long *) (inputData + offset + SIXTEEN_K); //pointer to sybol defined in Linker Script
    struct block* blocks = (struct block*)(inputData + offset + SIXTEEN_K + 8);

    uint32_t numWords = sizeof(block_t) / sizeof(uint32_t);
    uint32_t blockToEncode [numWords];

    uint32_t crc;
    
    uint16_t parityData[PARITY_SYMBOL_COUNT]; //used to print hex of parity symbols

    int idx, x;
    int parityStartIdx = SYMBOL_TABLE_WORDS - PARITY_SYMBOL_COUNT - 1;

    for (idx = 0; idx < *blockCount; idx++) {
        memcpy(blockToEncode, &(blocks[idx]), sizeof(block_t)-1);
        encode_rs((word_t*) blockToEncode);
        crc = CRC_CalcBlockCRC((uint32_t*)blockToEncode, numWords);
        blocks[idx].crc = crc;

        memcpy(parityData, &(blocks[idx].reed_solomon_data[parityStartIdx]), PARITY_SYMBOL_COUNT);
        
        //Printing this data for debugging purposes 
        printf("%08x,", crc);
        for (x = 0; x < PARITY_SYMBOL_COUNT; x++) {
            printf("%x", parityData[x]);
        }
        printf("\n");
    }

    //Write modified binary to new file
    fwrite(inputData, sizeof(char), inputFileLen, outputFile);

    free(inputData);
    return 0;
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
