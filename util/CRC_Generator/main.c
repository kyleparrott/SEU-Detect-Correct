#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "crcmodel.h"

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
}

//FROM:https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=https%3a%2f%2fmy%2est%2ecom%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fcortex_mx_stm32%2fCRC%20calculation%20in%20software&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=8435
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
