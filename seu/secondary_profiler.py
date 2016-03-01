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
import subprocess

NUMBER_TEXT_SECTIONS = 4
NUM_IGNORE_BYTES = 16 #number of bytes to ignore from start of section 

current_path = os.path.dirname(os.path.abspath(__file__))
hex_dump_filename = os.path.join(current_path, 'gen', 'text_hex_dump_%s.hex')
crc_exe_path = os.path.join(current_path, '..', 'build', 'crcGenerator')

section_name = 'text%s'

class TextSectionObj:
    def __init__(self, name, number):
        self.name = name
        self.contents = ''
        self.crc = 0

section_arr = []
for idx in range(1, NUMBER_TEXT_SECTIONS):
    name = section_name % idx
    section_arr.append(TextSectionObj(name, idx))


for x in range(1, NUMBER_TEXT_SECTIONS):
    file_name = hex_dump_filename % x
    content_str = ''
    with open(file_name, 'r') as hex_file:
        for line in hex_file:
            content_str += line.strip()
    start_idx = 2 * NUM_IGNORE_BYTES # contents are hex encoded, 2 hex chars per byte
    reduced_contents = content_str[start_idx:] #drop the block header from the hex dump
    # output = subprocess.check_output([crc_exe_path])
    # crc = output.decode('utf-8')
    # print ('crc: ', crc)



