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
import os
import re
import subprocess

thisDir = os.path.dirname(os.path.abspath(__file__))
genDir = os.path.join(thisDir, 'gen')
crcGenerator = os.path.join(os.path.dirname(thisDir), 'build', 'crcGenerator')

cfileName = os.path.join(genDir, 'secondary_seu_headers.c')
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
nameArray = [] #Prevent two functions with the same section name from being added twice. Will come up with better solution later


#Read in the starting address from the hexDump
startAddress = 0
with open(os.path.join(genDir, 'startAddr.data')) as startAddrFile:
    for line in startAddrFile:
        startAddress = int(line, 16)

#Read the hex dump into an array that indexes each address against it's hex value
x = 1
with open(os.path.join(genDir, 'textHexDump.hex'), 'r') as hexFile:
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

regex = re.compile('[^a-zA-Z0-9_]')

#Get the starting address, length and name of each function and stor it into the sectionDataArray.
with open(os.path.join(genDir, 'addressDump.data'), 'r') as addressFile:
    for line in addressFile:
        addressData = line.split()

        if int(addressData[1], 16) != 0 and 'handler' not in addressData[2].lower() and addressData[2] not in nameArray: #if the size of the function is not zero and its name does not include handler
            name = regex.sub('', addressData[2])
            nameArray.append(regex.sub('', addressData[2]))       
            sectionDataArray.append(SectionDataObj(name, addressData[0], addressData[1]))

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
    out = subprocess.check_output([crcGenerator, finalContents])
    elem.crc = int(out)
    elem.contents = finalContents
    finalContents = ''

with open(cfileName, 'w') as cFile:
    cFile.write(cIncludes)
    x = 0
    for entry in sectionDataArray: ## this whole block is going to change
        name = cSectionName % str(x)
        cFile.write(cEntry % (entry.name, entry.name, entry.crc, entry.size))
        x += 1

print ('successfully generated c symbol file %s with Final values' % cfileName)
