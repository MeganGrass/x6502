
------------------------

x6502
Ricoh 2A03 Disassembler

V.1.2.0.1
Jan 17, 2020

Megan Grass
https://www.romhacking.net/utilities/1146/

------------------------

x6502 is a simple commandline utility used to disassemble NES ROM data.

Disassembled output can be used in a variety of assemblers.

Commands can be given in no particular order and are not case-sensitive.

Please view "usage.txt" for usage information.

------------------------

EXAMPLES:

x6502 org 0xC7B5 start 0x03C7C5 finish 0x03C85E rom "C:\My Folder\Mega Man 2.nes" out "C:\My Folder\Init.s"

x6502 start 0x03C7C5 finish 0x03C85E org 0xC7B5 rom ".\Mega Man 2.nes" out ".\Init.s"

x6502 org 0xC7B5 start 0x03C7C5 finish 0x03C85E rom ".\SubDirectory\Mega Man 2.nes" out "C:\Init.s"

x6502 label comment org 0xC7B5 start 0x03C7C5 finish 0x03C85E rom ".\Mega Man 2.nes" out ".\Init.s"

x6502 org 0xC7B5 start 0x03C7C5 finish 0x03C85E rom ".\Mega Man 2.nes" out ".\Init.s" label comment

APPEND EXAMPLE:
x6502 org 0xC000 start 0x03C010 finish 0x03C6E8 rom ".\Mega Man 3.nes" out ".\source.s" label comment address
x6502 org 0xC6D8 start 0x03C6E8 finish 0x03C75C rom ".\Mega Man 3.nes" append ".\source.s" data
