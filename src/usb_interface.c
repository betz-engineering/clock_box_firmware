#include "ch32v20x.h"
#include "main.h"
#include "tusb.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
uint8_t line_buf[128] = {0};
int line_buf_n = 0;

static void process_command() {
    if (memcmp(line_buf, "*IDN?", 5) == 0) {
        tud_cdc_n_write(0, TITLE_STR, sizeof(TITLE_STR) - 1);
        tud_cdc_n_write(0, "\n\r", 1);
    } else {
        tud_cdc_n_write(0, "??? ", 4);
        tud_cdc_n_write(0, line_buf, line_buf_n);
    }
}

void cdc_task(void) {
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    if (tud_cdc_n_connected(0)) {
        if (tud_cdc_n_available(0)) {
            line_buf_n += tud_cdc_n_read(0, &line_buf[line_buf_n], sizeof(line_buf) - line_buf_n);

            if (line_buf[line_buf_n - 1] == '\r' || line_buf[line_buf_n - 1] == '\n' ||
                line_buf_n >= sizeof(line_buf)) {
                process_command();
                tud_cdc_n_write_flush(0);
                line_buf_n = 0;
            }
        }
    }
}

// Invoked when device is mounted
void tud_mount_cb(void) {}

// Invoked when device is unmounted
void tud_umount_cb(void) {}

// Invoked when cdc when line state changed e.g connected/disconnected
// Use to reset to DFU when disconnect with 1200 bps
void tud_cdc_line_state_cb(uint8_t instance, bool dtr, bool rts) {
    (void)rts;

    // DTR = false is counted as disconnected
    // if (!dtr) {
    //     // touch1200 only with first CDC instance (Serial)
    //     if (instance == 0) {
    //         cdc_line_coding_t coding;
    //         tud_cdc_get_line_coding(&coding);
    //         if (coding.bit_rate == 1200) {
    //             board_reset_to_bootloader();
    //         }
    //     }
    // }
}
