#initial_profiler.py

linkScriptTemplate = 'template_script.ld'
generatedLinkerScript = 'gen/secondary_seu_link.ld'
elfInput = 'gen/readElf.data'

cfileName = 'gen/secondary_seu_headers.c'
cIntro = '#include <stdint.h>\n#include "block_header.h"\n\n'

cEntry = 'blockHeader BlockHeader%s __attribute__((section (".header.%s"))) = {%iu, %i};\n'
cSectionName = '.header.%s'

entryFormat = '    *(.text.%s)\n'
headerEntryFormat = '    *(%s)\n'


class SectionDataObj:
    def __init__(self, name, address, size):
        self.name = name
        self.address = int(address,16)
        self.size = int(size)
        self.crc = 0  # placeholder

sectionDataArray = []

with open('gen/addressDump.data', 'r') as addressFile:
    for line in addressFile:
        addressData = line.split()
        if int(addressData[1], 16) != 0:
            sectionDataArray.append(SectionDataObj(addressData[2], addressData[0], addressData[1]))

# Write linkerscript to file
with open(linkScriptTemplate, 'r') as templateFile:
        headerTemplateData = [next(templateFile) for n in range(34)] #fixed file lengths
        tailTemplateData = [next(templateFile) for n in range(106)]
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
