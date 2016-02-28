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
#include "crcmodel.h"
#include <reed_solomon.h>

static uint32_t CRC_CalcBlockCRC(uint32_t *buffer, uint32_t words);


int main(int argc, char** argv) {
    unsigned long argLen = strlen(argv[1]);
    unsigned long remainder = argLen % 8;
    unsigned long numWords = argLen / 8;
    if (remainder != 0){
        numWords++;
    }
    uint32_t inputArray[numWords];
    int index, start = 0, length = 8;

    char intString[9];
    intString[8] = '\0';
    for (index = 0; index < numWords; index++) {
        memcpy(&intString, argv[1] + start, length);
        sscanf(intString, "%x", inputArray+index);
        memset(intString, 0, 8);
        start += 8;
    }
    uint32_t res = CRC_CalcBlockCRC(inputArray, numWords);
    printf("%02u\n", res);
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
