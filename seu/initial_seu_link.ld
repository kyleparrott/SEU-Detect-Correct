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
    flash00 : ORIGIN = 0x08000000, LENGTH = 16K
    flash01 : ORIGIN = 0x08004000, LENGTH = 16K
    flash02 : ORIGIN = 0x08008000, LENGTH = 16K
    flash03 : ORIGIN = 0x0800C000, LENGTH = 16K
    flash04 : ORIGIN = 0x08010000, LENGTH = 64K
    flash05 : ORIGIN = 0x08020000, LENGTH = 64K
    flash06 : ORIGIN = 0x08030000, LENGTH = 64K
    flash07 : ORIGIN = 0x08040000, LENGTH = 64K
    flash08 : ORIGIN = 0x08050000, LENGTH = 64K
    flash09 : ORIGIN = 0x08060000, LENGTH = 64K
    flash10 : ORIGIN = 0x08070000, LENGTH = 64K
    flash11 : ORIGIN = 0x08080000, LENGTH = 64K
    flash12 : ORIGIN = 0x08090000, LENGTH = 64K
    flash13 : ORIGIN = 0x080A0000, LENGTH = 64K
    flash14 : ORIGIN = 0x080B0000, LENGTH = 64K
    flash15 : ORIGIN = 0x080C0000, LENGTH = 64K
    flash16 : ORIGIN = 0x080D0000, LENGTH = 64K
    flash17 : ORIGIN = 0x080E0000, LENGTH = 128K
    /*   flash18 : ORIGIN = 0x080F0000, LENGTH = 64K */
}

SECTIONS
{
	.text00 : { . = ALIGN(4); *(.flash00hdr); *(.flash00func); } >flash00
	.text01 : { . = ALIGN(4); *(.flash01hdr); *(.flash01func); } >flash01
	.text02 : { . = ALIGN(4); *(.flash02hdr); *(.flash02func); } >flash02
	.text03 : { . = ALIGN(4); *(.flash03hdr); *(.flash03func); } >flash03
	.text04 : { . = ALIGN(4); *(.flash04hdr); *(.flash04func); } >flash04
	.text05 : { . = ALIGN(4); *(.flash05hdr); *(.flash05func); } >flash05
	.text06 : { . = ALIGN(4); *(.flash06hdr); *(.flash06func); } >flash06
	.text07 : { . = ALIGN(4); *(.flash07hdr); *(.flash07func); } >flash07
	.text08 : { . = ALIGN(4); *(.flash08hdr); *(.flash08func); } >flash08
	.text09 : { . = ALIGN(4); *(.flash09hdr); *(.flash09func); } >flash09
	.text10 : { . = ALIGN(4); *(.flash10hdr); *(.flash10func); } >flash10
	.text11 : { . = ALIGN(4); *(.flash11hdr); *(.flash11func); } >flash11
	.text12 : { . = ALIGN(4); *(.flash12hdr); *(.flash12func); } >flash12
	.text13 : { . = ALIGN(4); *(.flash13hdr); *(.flash13func); } >flash13
	.text14 : { . = ALIGN(4); *(.flash14hdr); *(.flash14func); } >flash14
	.text15 : { . = ALIGN(4); *(.flash15hdr); *(.flash15func); } >flash15
	.text16 : { . = ALIGN(4); *(.flash16hdr); *(.flash16func); } >flash16
	.text17 : { . = ALIGN(4); *(.flash17hdr); *(.flash17func); } >flash17
	/*	.text18 : { . = ALIGN(4); *(.flash18hdr); *(.flash18func); } >flash18 */


    .text :
    {
        . = ALIGN(4);
        _text = .;
        PROVIDE(stext = .);
        KEEP(*(.isr_vector))
        /*(.text)
        *(.text*)*/
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

    } >flash17

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
