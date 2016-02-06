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
import subprocess

linkScriptTemplate = "template_script.ld"
generatedLinkerScript = "gen/gen_stm32F4_flash.ld"
elfInput = 'gen/readElf.data'

cfileName = 'gen/secondary_seu_headers.c'
cIncludes = '#include "block_header.h"\n\n'

cEntry = 'blockHeader header%s __attribute__((section (".header.%s"))) = {%su, %i};\n'
cSectionName = '.header.%s'
functionEntryFmt = '    *(.text.%s)\n'
headerEntryFmt = '    *(%s)\n'


class AddressObj:
    def __init__(self, address, value):
        self.address = int(address, 16)
        self.value = str(value)

class SectionDataObj:
    def __init__(self, name, address, size):
        self.name = name
        self.address = int(address,16)
        self.size = int(size)
        self.crc = 0

sectionDataArray = []
hexArray = []

#Read in the starting address from the hexDump
startAddress = 0
with open('gen/startAddr.data') as startAddrFile:
    for line in startAddrFile:
        startAddress = int(line, 16)

#Read the hex dump into an array that indexes each address against it's hex value
x = 1
with open('gen/textHexDump.hex', 'r') as hexFile:
    for line in hexFile:
        if x < 3:
            indexAddress = startAddress
            x+=1
        else:
            entry = line.split()
            if len(entry) < 1 or 'dump' in entry[0]:
                pass
            else:
                entry = re.findall('..', entry[0])
                for elem in entry:
                    hexArray.append(AddressObj(hex(indexAddress), elem))
                    indexAddress += 1

#Get the starting address, length and name of each function and stor it into the sectionDataArray.
with open('gen/addressDump.data', 'r') as addressFile:
    for line in addressFile:
        addressData = line.split()
        if int(addressData[1], 16) != 0:
            sectionDataArray.append(SectionDataObj(addressData[2], addressData[0], addressData[1]))


#Loop through each function and rebuild it's contents from the hexdump
#then calculate the crc from the crc util and store that crc into the function metadata.
finalContents = ''
for elem in sectionDataArray:
    tempList = []
    tempstr = ''
    for e in hexArray:
        if e.address == elem.address: #get start index of section
            index = hexArray.index(e)
            final = index + elem.size #byte to words.
            if not elem.size == 0:
                for x in range(index, final):
                    tempList.insert(0, hexArray[x].value)
                    if len(tempList) == 4: # reversing endian of the hexdump to match memory layout of stm32f4
                        tempstr = ''.join(tempList)
                        finalContents += tempstr
                        tempList = []
            else:
                hexContents = '0'
    process = subprocess.Popen(['./crcGenerator', finalContents], stdout=subprocess.PIPE)
    out, err = process.communicate()
    crc = out
    elem.crc =  str(crc)[2:-3] #Need to fix to less-hacky solution
    elem.contents = finalContents
    finalContents = ''


with open(cfileName, 'w') as cFile:
    cFile.write(cIncludes)
    x = 0
    for entry in sectionDataArray: ## this whole block is going to change
        name = cSectionName % str(x)
        cFile.write(cEntry % (entry.name, entry.name, entry.crc, entry.size))
        x += 1

print ('\nsuccessfully generated c symbol file %s with Final values\n' % cfileName)
