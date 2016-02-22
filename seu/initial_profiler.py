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

linkScriptTemplate = os.path.join('seu', 'initial_seu_link.ld')
generatedLinkerScript = os.path.join('seu', 'gen', 'secondary_seu_link.ld')
elfInput = os.path.join('seu', 'gen', 'readElf.data')

cfileName = os.path.join('seu', 'gen', 'secondary_seu_headers.c')
cIncludes = '#include <stdint.h>\n#include "block_header.h"\n\n'

cEntry = 'blockHeader BlockHeader%s __attribute__((section (".FlashHeader%s"))) = {%i, %i};\n'
cSectionName = '.header.%s'
functionMap = os.path.join('seu', 'gen', 'addressDump.data')

sectionFormat = '''
    .text%s : 
    {
        . = ALIGN(4);
        *(.FlashHeader%s)
%s    
    } >flash%02i'''


functionEntryFormat = '        *(.text.%s)\n'

SIXTEEN_K = 16 * 1024
SIXTY_FOUR_K = 64 * 1024
ONE_TWENTY_EIGHT_K = 128 * 1024

HEADERSIZE = 16 #bits (Accounting for the 2 bytes pading added to the 6 byte header to word alignment)
WORDSIZE = 32 #bits


class LSSection: 
    def __init__(self, name, number, length):
        self.name = name
        self.number = number
        self.length = length

class SectionDataObj:
    def __init__(self, name, address, length):
        self.name = name
        self.address = int(address,16)
        self.length = int(length)

sectionDataArray = []
nameArray = [] #Prevent two functions with the same section name from being added twice. Will come up with better solution later
regex = re.compile('[^a-zA-Z0-9_]')

linkScriptSectionArray = []

linkScriptSectionArray.append( LSSection('.text0', 0, SIXTEEN_K))
linkScriptSectionArray.append( LSSection('.text1', 1, SIXTEEN_K))
linkScriptSectionArray.append( LSSection('.text2', 2, SIXTEEN_K))
linkScriptSectionArray.append( LSSection('.text3', 3, SIXTEEN_K))
linkScriptSectionArray.append( LSSection('.text4', 4, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text5', 5, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text6', 6, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text7', 7, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text8', 8, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text9', 9, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text10', 10, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text11', 11, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text12', 12, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text13', 13, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text14', 14, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text15', 15, SIXTY_FOUR_K))
linkScriptSectionArray.append( LSSection('.text16', 16, SIXTY_FOUR_K))
# linkScriptSectionArray.append( LSSection('.text17', 18, ONE_TWENTY_EIGHT_K)) Where everything else is being dumped at the moment
# linkScriptSectionArray.append( LSSection('.text18', 19, SIXTY_FOUR_K)) 

#Read function address map file into an array
with open(functionMap, 'r') as addressFile:
    for line in addressFile:
        addressData = line.split()
        if int(addressData[1], 16) != 0 and 'handler' not in addressData[2].lower() and addressData[2] not in nameArray: #if the size of the function is not zero and its name does not include handler
            nameArray.append(addressData[2])
            sectionDataArray.append(SectionDataObj(regex.sub('', addressData[2]), addressData[0], addressData[1]))

# Write linkerscript to file
with open(linkScriptTemplate, 'r') as templateFile:
        headerTemplateData = [next(templateFile) for n in range(45)] #fixed file lengths
        tailTemplateData = [next(templateFile) for n in range(68)]
        with open(generatedLinkerScript, 'w') as outputFile:
            x = 0
            for line in headerTemplateData: #write first half of linker script
                outputFile.write('%s' % line)

            runningSize = 0 #Running total of size of functions placed in current section
            index = 0 # Current linker script section
            sectionString = '' # contents of the current LS section
            for entry in sectionDataArray:        #write individual sections to linker script
                if runningSize + entry.length < linkScriptSectionArray[x].length:
                    sectionString += functionEntryFormat % entry.name
                    runningSize += entry.length
                    runningSize += HEADERSIZE # Account for word aligned header size 
                    runningSize += entry.length % WORDSIZE #Try to compensate for alignment

                else: 
                    outputFile.write(sectionFormat % (linkScriptSectionArray[x].number, linkScriptSectionArray[x].number, sectionString, linkScriptSectionArray[x].number))
                    x+=1
                    runningSize = 0
            outputFile.write(sectionFormat % (linkScriptSectionArray[x].number, linkScriptSectionArray[x].number, sectionString, linkScriptSectionArray[x].number)) # Write final section to file 
            
            for index in range(x+1, len(linkScriptSectionArray)): #include the empty sections (if any) in the linkerscript 
                    outputFile.write(sectionFormat % (linkScriptSectionArray[index].number, linkScriptSectionArray[index].number, '', linkScriptSectionArray[index].number))

            for line in tailTemplateData:    #write second half of linkerscript
                outputFile.write('%s' % line)

with open(cfileName, 'w') as cFile:
    cFile.write(cIncludes)
    for entry in linkScriptSectionArray:
        cFile.write(cEntry % (entry.number, entry.number, 0, 0))

print ('successfully generated c symbol file %s with placeholder values' % cfileName)
