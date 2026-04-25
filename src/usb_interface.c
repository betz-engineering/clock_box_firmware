#include "ch32v20x.h"
#include "main.h"
#include "tusb.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
char line_buf[128] = {0};
int line_buf_n = 0;

static void process_command() {
    if (memcmp(line_buf, "*IDN?", 5) == 0) {
        tud_cdc_n_write(0, "Betz Engineering,clock_box,", 27);
        tud_cdc_n_write(0, "xxx", 3);  // TODO add serial number
        tud_cdc_n_write(0, ",", 1);
        tud_cdc_n_write(0, GIT_REV, sizeof(GIT_REV) - 1);
        tud_cdc_n_write(0, "\n", 1);
    } else if (memcmp(line_buf, "f?", 2) == 0) {
        f_set_to_buf(line_buf);
        tud_cdc_n_write(0, line_buf, strlen(line_buf));
        tud_cdc_n_write(0, " Hz\n", 4);
    } else if (memcmp(line_buf, "f ", 2) == 0) {
        int64_t tmp = strtoll(&line_buf[1], NULL, 0);
        if (set_f_set(tmp))
            tud_cdc_n_write(0, "OK\n", 3);
        else
            tud_cdc_n_write(0, "ERROR\n", 6);
    } else if (memcmp(line_buf, "p?", 2) == 0) {
        itoa(get_p_set(), line_buf, 10);
        tud_cdc_n_write(0, line_buf, strlen(line_buf));
        tud_cdc_n_write(0, "\n", 1);
    } else if (memcmp(line_buf, "p ", 2) == 0) {
        int tmp = strtol(&line_buf[1], NULL, 0);
        if (set_p_set(tmp))
            tud_cdc_n_write(0, "OK\n", 3);
        else
            tud_cdc_n_write(0, "ERROR\n", 6);
    } else if (memcmp(line_buf, "h?", 2) == 0) {
        tud_cdc_n_write(0, "*IDN? = identify\n", 17);
        tud_cdc_n_write(0, "f / f? = freq. [Hz]\n", 20);
        tud_cdc_n_write(0, "p / p? = power [0 - 63]\n", 24);
    } else {
        tud_cdc_n_write(0, "Unknown command. Use h? for help\n", 33);
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
