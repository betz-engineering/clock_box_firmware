#include "main.h"
#include "flash_slot.h"
#include "font.h"
#include "frame_buffer.h"
#include "graphics.h"
#include "lmx2572.h"
#include "missingplanet.h"
#include "profont.h"
#include "ssd1306_i2c.h"
#include "ssd1306_j.h"
#include "tusb.h"
#include "tusb_config.h"
#include "usb_interface.h"
#include "user_input.h"
#include <ch32v20x.h>
#include <ch32v20x_gpio.h>
#include <debug.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define N_DIGITS 10
#define CURSOR_TIMEOUT_VAL 5000  // [ms]

static int64_t g_f_set = 25000000;
static int g_pwr_a_set = 0x1f;
static bool g_pwr_a_on = true;
static int g_digit_select = 0;
static int64_t digit_multiplier = 1;

static t_nvm_state loaded_state = {0};
static t_f_plan g_plan = {0};

// Convert the 64 bit integer f_set to 10 characters
// Can't use snprintf as there is no 64 bit support :(
static void f_set_to_buf(char *char_buf) {
    int64_t temp = g_f_set;
    char_buf[N_DIGITS] = '\0';

    // Extract digits into char_buf from right to left
    for (int i = N_DIGITS - 1; i >= 0; i--) {
        if (temp <= 0) {
            char_buf[i] = ' ';
        } else {
            char_buf[i] = (char)((temp % 10) + '0');
            temp /= 10;
        }
    }
}

static void store_flash() {
    t_nvm_state current_state = {
        .f_set = g_f_set,
        .digit_select = g_digit_select,
        .pwr_a_set = g_pwr_a_set,
        .pwr_on_flags = g_pwr_a_on,
    };

    // Only store if something changed
    if (memcmp(&loaded_state, &current_state, sizeof(t_nvm_state)) != 0) {
        save_slot((uint8_t *)(&current_state));
        memcpy(&loaded_state, &current_state, sizeof(t_nvm_state));
    }
}

static void display_f_set(bool is_cursor) {
    static bool is_cursor_d = true;
    char char_buf[N_DIGITS + 1];
    f_set_to_buf(char_buf);

    // Micro-manage the gaps between number-groups
    const int gap = 3;
    int c_x = 0;
    init_from_header(&f_profont);
    c_x = push_str(c_x, FB_HEIGHT / 2, char_buf, 1, A_LEFT);
    c_x = push_str(c_x + gap, FB_HEIGHT / 2, &char_buf[1], 3, A_LEFT);
    c_x = push_str(c_x + gap, FB_HEIGHT / 2, &char_buf[4], 3, A_LEFT);
    c_x = push_str(c_x + gap, FB_HEIGHT / 2, &char_buf[7], 3, A_LEFT);

    // Draw a line above and below the selected digit
    if (is_cursor) {
        int x_start = (N_DIGITS - g_digit_select - 1) * 12;
        if (g_digit_select < 3)
            x_start += 3 * gap;
        else if (g_digit_select < 6)
            x_start += 2 * gap;
        else if (g_digit_select < 9)
            x_start += gap;
        int x_end = x_start + 9;
        draw_line(x_start, 0, x_end, 0);
        draw_line(x_start, 17, x_end, 17);
    }

    // When the cursor times out, store parameters to flash
    if (is_cursor_d && !is_cursor)
        store_flash();
    is_cursor_d = is_cursor;
}

int main() {
    SystemInit();
    peripherals_init();
    encoder_init();
    sys_tick_config(SystemCoreClock / 1000);  // sys tick every 1 ms

#if (SDI_PRINT == 1)
    SDI_Printf_Enable();
    // restore SysTick config overwritten by debug.c
    sys_tick_config(SystemCoreClock / 1000);
    delay_ms(1000);
#endif

    D("Hello World! This is clock_box!!\n");
    D("SystemClk: %ld Hz\n", SystemCoreClock);

    // Init tiny-USB
    tud_init(BOARD_TUD_RHPORT);

    lmx_init();
    // lmx_dump();

    ssd1306_i2c_init();
    delay_ms(30);  // Give the OLED some time to come up

    ssd1306_init();
    init_from_header(&f_profont);
    print_font_info();

    // Load last set-point from NVS
    if (load_slot((uint8_t *)(&loaded_state))) {
        g_f_set = loaded_state.f_set;
        g_digit_select = loaded_state.digit_select;
        g_pwr_a_on = loaded_state.pwr_on_flags & 1;
        g_pwr_a_set = loaded_state.pwr_a_set;
    }
    D("g_digit_select: %d\n", g_digit_select);
    D("g_pwr_a_on: %d\n", g_pwr_a_on);
    D("g_pwr_a_set: %d\n", g_pwr_a_set);

    bool f_set_changed = true;
    int cursor_timeout = millis() + CURSOR_TIMEOUT_VAL;
    bool is_cursor_on = DEBUG;
    bool update_screen = true;
    unsigned frame = 0;
    enum { M_ADJ_DIGITS, M_ADJ_POWER } mode_select = M_ADJ_DIGITS;

    while (1) {
        tud_task();
        cdc_task();

        poll_inputs();
        unsigned event_flags = get_event_flags();

        if (!is_cursor_on) {
            // We are in IDLE mode, if there was an event, enable the cursor and swallow it
            if (event_flags &
                (EV_ENC_SW_S | EV_ROCK_SW_S | EV_ROCK_B_P | EV_ROCK_A_P | EV_ROT_CCW | EV_ROT_CW)) {
                cursor_timeout = millis() + CURSOR_TIMEOUT_VAL;
                get_encoder_ticks(true);  // discard potential encoder tick
                is_cursor_on = true;
                update_screen = true;
            }
            if (millis() > 3000)
                continue;
        }

        if (event_flags & EV_ENC_SW_S) {
            g_pwr_a_on = !g_pwr_a_on;
            update_screen = true;
            cursor_timeout = millis() + CURSOR_TIMEOUT_VAL;
            lmx_set_output_enable(g_pwr_a_on, false);
        }

        if (mode_select == M_ADJ_DIGITS) {
            if (event_flags & EV_ROCK_B_P) {
                g_digit_select++;
                if (g_digit_select >= N_DIGITS)
                    g_digit_select = 0;
            }

            if (event_flags & EV_ROCK_A_P) {
                g_digit_select--;
                if (g_digit_select < 0)
                    g_digit_select = N_DIGITS - 1;
            }

            if (event_flags & (EV_ROCK_A_P | EV_ROCK_B_P) || frame == 0) {
                digit_multiplier = 1;
                for (int i = 0; i < g_digit_select; i++)
                    digit_multiplier *= 10;
                cursor_timeout = millis() + CURSOR_TIMEOUT_VAL;
                update_screen = true;
            }

            int enc = get_encoder_ticks(true);
            if (enc != 0) {
                g_f_set += enc * digit_multiplier;
                f_set_changed = true;
            }

            // Reset all digits on the right of digit_select
            if (event_flags & (EV_ROCK_SW_L | EV_ENC_SW_L)) {
                g_f_set -= g_f_set % (digit_multiplier);
                f_set_changed = true;
            }
            if (event_flags & EV_ROCK_SW_S) {
                mode_select = M_ADJ_POWER;
                update_screen = true;
            }

            if (f_set_changed) {
                // Enforce absolute frequency limits
                if (g_f_set < 12500000)
                    g_f_set = 12500000;
                if (g_f_set > 6400000000)
                    g_f_set = 6400000000;

                get_f_plan(g_f_set, &g_plan);
                print_f_plan(&g_plan);
                lmx_write_f_plan(&g_plan);

                if (frame == 0) {
                    lmx_set_output_enable(g_pwr_a_on, false);
                    lmx_set_outb_pwr(0);
                    lmx_set_outa_pwr(g_pwr_a_set);
                }

                f_set_changed = false;
                cursor_timeout = millis() + CURSOR_TIMEOUT_VAL;
                update_screen = true;
            }

        } else if (mode_select == M_ADJ_POWER) {
            if (event_flags & EV_ROCK_SW_S) {
                mode_select = M_ADJ_DIGITS;
                update_screen = true;
                cursor_timeout = millis() + CURSOR_TIMEOUT_VAL;
            }
            int enc = get_encoder_ticks(true);
            if (enc != 0) {
                g_pwr_a_set += enc;
                if (g_pwr_a_set > 0x3f)
                    g_pwr_a_set = 0x3f;
                if (g_pwr_a_set < 0)
                    g_pwr_a_set = 0;
                lmx_set_outa_pwr(g_pwr_a_set);
                update_screen = true;
                cursor_timeout = millis() + CURSOR_TIMEOUT_VAL;
            }
        }

        // Timeout for disabling the cursor
        if (millis() > cursor_timeout) {
            is_cursor_on = false;
            update_screen = true;
        }

        if (update_screen) {
            fill(0);
            display_f_set(is_cursor_on && (mode_select == M_ADJ_DIGITS));
            init_from_header(&f_missingplanet);

            if (millis() > 2500 || is_cursor_on) {
                if (g_digit_select <= 2 || !is_cursor_on)
                    push_str(FB_WIDTH, FB_HEIGHT, "Hz", 4, A_RIGHT);
                else if (g_digit_select <= 5)
                    push_str(FB_WIDTH, FB_HEIGHT, "kHz", 4, A_RIGHT);
                else if (g_digit_select <= 8)
                    push_str(FB_WIDTH, FB_HEIGHT, "MHz", 4, A_RIGHT);
                else if (g_digit_select <= 11)
                    push_str(FB_WIDTH, FB_HEIGHT, "GHz", 4, A_RIGHT);

                if (is_cursor_on && (mode_select == M_ADJ_POWER))
                    push_print(0, FB_HEIGHT, A_LEFT, "PWR [%d]", g_pwr_a_set);
                else
                    push_print(0, FB_HEIGHT, A_LEFT, "PWR  %d", g_pwr_a_set);

                push_print(60, FB_HEIGHT, A_LEFT, g_pwr_a_on ? "RF On" : "RF Off");
                update_screen = false;
            } else {
                int x = 200 - frame * 2;
                if (x <= (FB_WIDTH / 2))
                    x = FB_WIDTH / 2;
                push_str(x, FB_HEIGHT - 2, TITLE_STR, sizeof(TITLE_STR), A_CENTER);
            }

            ssd1306_refresh();
        }

        frame++;
    }
}
