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

linkScriptTemplate = 'seu/initial_seu_link.ld'
generatedLinkerScript = 'seu/gen/secondary_seu_link.ld'
elfInput = 'seu/gen/readElf.data'

cfileName = 'seu/gen/secondary_seu_headers.c'
cIntro = '#include <stdint.h>\n#include "block_header.h"\n\n'

cEntry = 'blockHeader BlockHeader%s __attribute__((section (".header.%s"))) = {%iu, %i};\n'
cSectionName = '.header.%s'

entryFormat = '        *(.text.%s)\n'
headerEntryFormat = '        *(%s)\n'


class SectionDataObj:
    def __init__(self, name, address, size):
        self.name = name
        self.address = int(address,16)
        self.size = int(size)
        self.crc = 0  # placeholder

sectionDataArray = []
nameArray = [] #Prevent two functions with the same section name from being added twice. Will come up with better solution later
regex = re.compile('[^a-zA-Z0-9_]')

with open('seu/gen/addressDump.data', 'r') as addressFile:
    for line in addressFile:
        addressData = line.split()
        if int(addressData[1], 16) != 0 and 'handler' not in addressData[2].lower() and addressData[2] not in nameArray: #if the size of the function is not zero and its name does not include handler
            nameArray.append(addressData[2])
            sectionDataArray.append(SectionDataObj(regex.sub('', addressData[2]), addressData[0], addressData[1]))

# Write linkerscript to file
with open(linkScriptTemplate, 'r') as templateFile:
        headerTemplateData = [next(templateFile) for n in range(33)] #fixed file lengths
        tailTemplateData = [next(templateFile) for n in range(61)]
        with open(generatedLinkerScript, 'w') as outputFile:
            x = 0
            for element in headerTemplateData: #write first half of linker script
                outputFile.write('%s' % element)

            for entry in sectionDataArray:        #write individual sections to linker script
                name = cSectionName % entry.name
                outputFile.write(headerEntryFormat % (name))
                outputFile.write(entryFormat % entry.name)
                x = x + 1
            for element in tailTemplateData:    #write second half of linkerscript
                outputFile.write('%s' % element)

with open(cfileName, 'w') as cFile:
    cFile.write(cIntro)
    x = 0
    for entry in sectionDataArray:
        name = cSectionName % str(x)
        cFile.write(cEntry % (entry.name, entry.name, int(entry.crc), entry.size))
        x += 1

print ('successfully generated c symbol file %s with placeholder values' % cfileName)
