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

static void put_hex(uint32_t val, uint8_t digits, char *p) {
    const char lut[] = "0123456789ABCDEF";

    for (int i = 0; i < 4 * digits; i += 8) {
        *p++ = lut[(val >> (i + 4)) % 16];
        *p++ = lut[(val >> i) % 16];
    }
}

// Will write R-_<unique_id in hex> (27 characters) to desc_str
static int ui_to_usb_get_serial(char *desc_str) {
    char *p = desc_str;
    *p++ = 'R';
    *p++ = PCB_REV;
    *p++ = '_';
    // Append 12 byte unique ID
    volatile uint32_t *ch32_uuid = ((volatile uint32_t *)0x1FFFF7E8UL);
    put_hex(ch32_uuid[0], 8, p);
    p += 8;
    put_hex(ch32_uuid[1], 8, p);
    p += 8;
    put_hex(ch32_uuid[2], 8, p);

    return 27;
}

static void process_command() {
    if (memcmp(line_buf, "*IDN?", 5) == 0) {
        tud_cdc_n_write(0, "Betz Engineering,clock_box,", 27);
        int n = ui_to_usb_get_serial(line_buf);
        tud_cdc_n_write(0, line_buf, n);
        tud_cdc_n_write_flush(0);
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
    } else if (line_buf[0] == '?') {
        tud_cdc_n_write(0, "*IDN? = identify\n", 17);
        tud_cdc_n_write(0, "f / f? = set / get frequency [Hz]\n", 34);
        tud_cdc_n_write_flush(0);
        tud_cdc_n_write(0, "p / p? = set / get power [0 - 63]\n", 34);
    } else {
        tud_cdc_n_write(0, "Unknown command. ? for help\n", 28);
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
