#include "flash_slot.h"
#include "ch32v20x.h"
#include "core_riscv.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#define PAGE_SIZE 0x400
#define FLASH_SLOT_SIZE_CHK (FLASH_SLOT_SIZE + 4)  // Add 1 word for the checksum
#define NUM_SLOTS (PAGE_SIZE / FLASH_SLOT_SIZE_CHK)

// Helper function to check if a slot is unwritten
static unsigned is_slot_empty(uint32_t slot_index) {
    uint32_t *slot_ptr = (uint32_t *)(FLASH_SAVE_ADDR + (slot_index * FLASH_SLOT_SIZE_CHK));

    for (int i = 0; i < (FLASH_SLOT_SIZE_CHK / 4); i++)
        if (slot_ptr[i] != 0xe339e339)  // Magic number indicating empty
            return 0;                   // Found data, slot is not empty
    return 1;
}

// Calculates an XOR checksum
static uint32_t calculate_checksum(uint32_t *buf) {
    uint32_t checksum = 0;
    for (int i = 0; i < (FLASH_SLOT_SIZE / 4); i++)
        checksum ^= buf[i];

    return checksum;
}

int load_slot(uint8_t *buf) {
    // Scan backwards from the last slot to find the newest data
    for (int i = NUM_SLOTS - 1; i >= 0; i--) {
        if (!is_slot_empty(i)) {
            memcpy(buf, (uint8_t *)(FLASH_SAVE_ADDR + (i * FLASH_SLOT_SIZE_CHK)), FLASH_SLOT_SIZE);
            uint32_t *chk =
                (uint32_t *)(FLASH_SAVE_ADDR + (i * FLASH_SLOT_SIZE_CHK) + FLASH_SLOT_SIZE);
            if (*chk == calculate_checksum((uint32_t *)buf)) {
                D("load_slot(%d): success!\n", i);
                return 1;
            } else {
                D("load_slot(%d): checksum failed\n", i);
            }
        }
    }
    D("load_slot() failed: all slots empty\n");
    return 0;
}

void save_slot(uint8_t *buf) {
    int target_slot = -1;

    // Scan forward to find the first empty slot
    for (int i = 0; i < NUM_SLOTS; i++) {
        if (is_slot_empty(i)) {
            target_slot = i;
            break;
        }
    }

    D("save_slot() target: %d\n", target_slot);

    __disable_irq();
    FLASH_Unlock();

    // If no empty slot was found, erase the page and start over at slot 0
    if (target_slot == -1) {
        FLASH_ErasePage(FLASH_SAVE_ADDR);
        target_slot = 0;
    }

    uint32_t *data_ptr = (uint32_t *)buf;
    uint32_t write_addr = FLASH_SAVE_ADDR + (target_slot * FLASH_SLOT_SIZE_CHK);

    for (int i = 0; i < (FLASH_SLOT_SIZE / 4); i++)
        FLASH_ProgramWord(write_addr + i * 4, data_ptr[i]);
    FLASH_ProgramWord(write_addr + FLASH_SLOT_SIZE, calculate_checksum((uint32_t *)buf));

    FLASH_Lock();
    __enable_irq();
}
