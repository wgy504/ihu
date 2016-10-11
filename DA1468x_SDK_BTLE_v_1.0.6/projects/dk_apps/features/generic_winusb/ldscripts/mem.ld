/* Linker script to configure memory regions. 
 * Need modifying for a specific board. 
 *   ROM.ORIGIN: starting address of read-only RAM area
 *   ROM.LENGTH: length of read-only RAM area
 *   RetRAM0.ORIGIN: starting address of retained RAM area
 *   RetRAM0.LENGTH: length of retained RAM area
 *   RAM.ORIGIN: starting address of read-write RAM area
 *   RAM.LENGTH: length of read-write RAM area
 */

MEMORY
{
	ROM (rx)     : ORIGIN = 0x7FC0000,       LENGTH = 64K
	RetRAM0 (rwx): ORIGIN = 0x7FC0000 + 64K, LENGTH = 16K
	RetRAM1 (rwx): ORIGIN = 0x7FC0000 + 80K, LENGTH = 16K
	RAM (rw)     : ORIGIN = 0x7FC0000 + 96K, LENGTH = 24K
}
