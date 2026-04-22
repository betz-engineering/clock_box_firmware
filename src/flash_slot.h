#pragma once
#include <stdint.h>

// Simple wear leveling
// Divide the flash page into N slots. Write them sequentially. Always load
// the last slot by checking if a slot is empty or not.
// When all slots are written, erase the whole page and start again.

#define FLASH_SAVE_ADDR 0x08007C00  // last page in 32 kB program memory
// #define FLASH_SAVE_ADDR 0x08008000  // first page in slow (inofficial?) 128 kB flash
#define FLASH_SLOT_SIZE 8  // [bytes] better be divisible by 4

// Loads the last slot into buf. Writes FLASH_SLOT_SIZE bytes into buf.
// Returns 1 on success, 0 on failure (or empty flash)
int load_slot(uint8_t *buf);

// writes FLASH_SLOT_SIZE + 4 byte checksum into the next slot.
// Erases flash page if all slots are full.
void save_slot(uint8_t *buf);
