#
# SEU detection and correction
# Copyright (C) 2015 Nano Avionics
#
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import re
import os

headLinkScriptTemplate = os.path.join('seu', 'gen', 'template_half_0.ld')
tailLinkScriptTemplate = os.path.join('seu', 'gen', 'template_half_1.ld')
generatedLinkerScript = os.path.join('seu', 'gen', 'secondary_seu_link.ld')
elfInput = os.path.join('seu', 'gen', 'readElf.data')
functionMap = os.path.join('seu', 'gen', 'fullMap.data')

functionEntryFormat =    '        *(.text.%s)\n'
functionPadFormat =      '        . += %s;\n'
blockFinishAndStartNew = '        . += {0};\n/****** Block {1} ******/\n'

ALIGNTO = 4;
BLOCKSIZE = 13312; # bytes. Exactly equals 6656 words or 3328 dwords.
PAYLOADSIZE = 13286;
BLOCKTRAILER = BLOCKSIZE - PAYLOADSIZE;
PROFILE0_BLOCK = 0;
PROFILE1_BLOCK = 3;
PROFILE2_BLOCK = 4;

class FunctionDataObj:
    def __init__(self, name, length, align):
        self.name = name
        self.length = int(length)
        self.align = int(align)
        self.bytes = int(length) + int(align)

functionDataArray = []

regex = re.compile('[^a-zA-Z0-9_]')

lineRegex = re.compile('08\S{6}\s')

#Read function map file into an array
with open(functionMap, 'r') as functionFile:
    for line in functionFile:
        functionData = line.split()
        padding = int(functionData[0]) % ALIGNTO
        if (padding > 0):
            padding = ALIGNTO - padding;
        functionDataArray.append(FunctionDataObj(functionData[1], functionData[0], padding))

headData = ''
with open (headLinkScriptTemplate, 'r') as headTemplateFile:
    for line in headTemplateFile:
        headData += line

tailData = ''
with open (tailLinkScriptTemplate, 'r') as tailTemplateFile:
    for line in tailTemplateFile:
        tailData += line

def IndexOf(value, arr):
    for index, item in enumerate(arr):
        if item.name == value:
            return index, item
    return -1

def getBiggestFunction(maxSize, arr):
    itemLen = -1
    itemIndex = -1
    for index, item in enumerate(arr):
        if ((item.bytes > itemLen) and (item.bytes <= maxSize)):
            itemIndex = index
            itemLen = item.bytes
    return itemIndex

to_section = [1, 1, 2, 3, 4]

# Write linkerscript to file
with open(generatedLinkerScript, 'w') as outputFile:
    for line in headData: #write first half of linker script
        outputFile.write('%s' % line)

    idx, s1 = IndexOf('section1_profile_func_enter', functionDataArray)
    del functionDataArray[idx]
    idx, s2 = IndexOf('section2_profile_func_enter', functionDataArray)
    del functionDataArray[idx]
    idx, s3 = IndexOf('section3_profile_func_enter', functionDataArray)
    del functionDataArray[idx]

    blknum = 0
    outputFile.write(functionEntryFormat % s1.name)
    if s1.align > 0:
        outputFile.write(functionPadFormat % s1.align)
    bytesAvailable = PAYLOADSIZE - s1.bytes

    while len(functionDataArray) > 0:
        idx = getBiggestFunction(bytesAvailable, functionDataArray)
        if (idx < 0):			 # could not find a function that will fit
            if (bytesAvailable == PAYLOADSIZE): # remaining functions are bigger than PAYLOADSIZE
                for elem in functionDataArray:
                    print('FUNCTION TOO BIG: {0} {1} {2}\n' .format(elem.length, elem.bytes, elem.name))
                raise Exception

            blknum += 1
            blktrailer = bytesAvailable + BLOCKTRAILER
            outputFile.write(blockFinishAndStartNew .format(blktrailer, blknum))
            bytesAvailable = PAYLOADSIZE

            if (blknum == 3):
                outputFile.write(functionEntryFormat % s2.name)
                if s2.align > 0:
                    outputFile.write(functionPadFormat % s2.align)
                bytesAvailable -= s2.bytes
            elif (blknum == 4):
                outputFile.write(functionEntryFormat % s3.name)
                if s3.align > 0:
                    outputFile.write(functionPadFormat % s3.align)
                bytesAvailable -= s3.bytes
        else:
            fd = functionDataArray[idx]
            outputFile.write(functionEntryFormat % fd.name)
            if (fd.align > 0):
                outputFile.write(functionPadFormat % fd.align)
            bytesAvailable -= fd.bytes
            del functionDataArray[idx]







    for line in tailData:    #write second half of linkerscript
        outputFile.write('%s' % line)

print ('successfully generated linker script')
