/* SEU detection and correction
 * Copyright (C) 2015 Nano Avionics
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

MEMORY
{
    sram (W!RX) : ORIGIN = 0x20000000, LENGTH = 192K
    CCMRAM (xrw) : ORIGIN = 0x60000000, LENGTH = 0K
    flash (xr) : ORIGIN = 0x08000000, LENGTH = 1024K
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _text = .;
        PROVIDE(stext = .);
        KEEP(*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata .rodata.*)
        *(.gnu.linkonce.t.*)
        *(.glue_7)
        *(.glue_7t)
        *(.gcc_except_table)
        *(.gnu.linkonce.r.*)
        . = ALIGN(4);
        _exit = .;
        _etext = .;
		_sidata = _etext;
        PROVIDE(etext = .);
     		_fini = . ;
				*(.fini)

    } >flash

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.ramfunc .ramfunc.* .fastrun .fastrun.*)
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        . = ALIGN(4);
        _edata = .;
    } >sram

		.ARM.extab :
		{
		    *(.ARM.extab*)
		} >sram

		__exidx_start = .;
		.ARM.exidx :
		{
		    *(.ARM.exidx*)
		} >sram
		__exidx_end = .;

    PROVIDE( flash_used_size = SIZEOF(.text) + SIZEOF(.data) + SIZEOF(.ARM.extab) + SIZEOF(.ARM.exidx) );

    .bss : {
		. = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } >sram

    end = .;
    PROVIDE( _estack = 0x20020000 );
}

ENTRY(Reset_Handler)
