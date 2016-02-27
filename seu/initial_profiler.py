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
functionMap = os.path.join('seu', 'gen', 'fullMap.data')

sectionFormat = '''
    .text%s :
    {
        . = ALIGN(4);
        *(.flash%shdr);
        *(.flash%sfunc);
%s
    } >flash%s
'''


functionEntryFormat = '        *(.text.%s)\n'

SIXTEEN_K = 12 * 1024
SIXTY_FOUR_K = 60 * 1024
ONE_TWENTY_EIGHT_K = 124 * 1024

HEADERSIZE = 72 # bytes sizeof(block_header_t)
WORDSIZE = 4 # bytes sizeof(int)

class LSSection:
    def __init__(self, name, number, length):
        self.name = name
        self.number = number
        self.length = length

class FunctionDataObj:
    def __init__(self, name, length):
        self.name = name
        self.length = int(length)

functionDataArray = []

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
# linkScriptSectionArray.append( LSSection('.text15', 15, SIXTY_FOUR_K))
# linkScriptSectionArray.append( LSSection('.text16', 16, SIXTY_FOUR_K))
# linkScriptSectionArray.append( LSSection('.text17', 17, ONE_TWENTY_EIGHT_K)) # Where everything else is being dumped at the moment
# linkScriptSectionArray.append( LSSection('.text18', 18, SIXTY_FOUR_K))

lineRegex = re.compile('08\S{6}\s')

#Read function map file into an array
with open(functionMap, 'r') as functionFile:
    for line in functionFile:
        functionData = line.split()
        functionDataArray.append(FunctionDataObj(functionData[1], functionData[0]))

# Write linkerscript to file
with open(linkScriptTemplate, 'r') as templateFile:
        headerTemplateData = [next(templateFile) for n in range(47)] #fixed file lengths
        tailTemplateData = [next(templateFile) for n in range(68)]
        with open(generatedLinkerScript, 'w') as outputFile:
            x = 0
            for line in headerTemplateData: #write first half of linker script
                outputFile.write('%s' % line)

            x = 0 # Current linker script section
            sec_num = linkScriptSectionArray[x].number
            functionString = '' # contents of the current LS section
            runningSize = HEADERSIZE #Running total size of functions placed in current section
            for entry in functionDataArray:        #write individual functions to linker script
                # Pad to .ALIGN(4) and add function length
                bytesOverAlignment = runningSize % WORDSIZE
                if (bytesOverAlignment > 0):
                    functionSize = WORDSIZE - bytesOverAlignment + entry.length
                else:
                    functionSize = entry.length
                if (runningSize + functionSize) >= linkScriptSectionArray[x].length:
                    outputFile.write(sectionFormat % (sec_num, sec_num, sec_num, functionString, sec_num))
                    x += 1
                    sec_num = linkScriptSectionArray[x].number
                    functionString = ''
                    runningSize = HEADERSIZE
                    functionSize = entry.length # Remove any padding, header is always aligned
                functionString += functionEntryFormat % entry.name
                runningSize += functionSize
            outputFile.write(sectionFormat % (sec_num, sec_num, sec_num, functionString, sec_num))

            # for index in range(x + 1, len(linkScriptSectionArray)): #include the empty sections (if any) in the linkerscript
                    # outputFile.write(sectionFormat % (linkScriptSectionArray[index].number, linkScriptSectionArray[index].number, '', linkScriptSectionArray[index].number))

            for line in tailTemplateData:    #write second half of linkerscript
                outputFile.write('%s' % line)

print ('successfully generated linker script')
