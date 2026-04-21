#include "main.h"
#include "font.h"
#include "frame_buffer.h"
#include "graphics.h"
#include "lmx2572.h"
#include "profont.h"
#include "ssd1306_i2c.h"
#include "ssd1306_j.h"
#include "user_input.h"
#include <ch32v20x.h>
#include <ch32v20x_gpio.h>
#include <debug.h>
#include <stdint.h>
#include <stdio.h>

#define N_DIGITS 10
static int64_t f_set = 25000000;
static int digit_select = 0;
static int64_t digit_multiplier = 1;

// Convert the 64 bit integer f_set to 10 characters
// Can't use snprintf as there is no 64 bit support :(
static void f_set_to_buf(char *char_buf) {
    int64_t temp = f_set;
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

static void display_f_set() {
    char char_buf[N_DIGITS + 1];
    f_set_to_buf(char_buf);

    // Micro-manage the gaps between number-groups
    const int gap = 3;
    int c_x = 0;
    init_from_header(&f_profont);
    c_x = push_str(c_x, FB_HEIGHT / 2, char_buf, 1, A_LEFT) + gap;
    c_x = push_str(c_x, FB_HEIGHT / 2, &char_buf[1], 3, A_LEFT) + gap;
    c_x = push_str(c_x, FB_HEIGHT / 2, &char_buf[4], 3, A_LEFT) + gap;
    c_x = push_str(c_x, FB_HEIGHT / 2, &char_buf[7], 3, A_LEFT);

    // Draw a line above and below the selected digit
    int x_start = (N_DIGITS - digit_select - 1) * 12;
    if (digit_select < 3)
        x_start += 3 * gap;
    else if (digit_select < 6)
        x_start += 2 * gap;
    else if (digit_select < 9)
        x_start += gap;
    int x_end = x_start + 9;
    draw_line(x_start, 0, x_end, 0);
    draw_line(x_start, 17, x_end, 17);
}

int main() {
    SystemInit();
    Delay_Init();
    gpio_init();
    encoder_init();

#if (SDI_PRINT == 1)
    SDI_Printf_Enable();
    Delay_Ms(1000);
#endif

    printf("Hello World! This is clock_box!!\n");
    printf("SystemClk: %ld Hz\n", SystemCoreClock);

    lmx_init();
    lmx_dump();

    ssd1306_i2c_init();
    Delay_Ms(10);  // Give the OLED some time to come up

    ssd1306_init();
    init_from_header(&f_profont);
    print_font_info();

    bool f_set_changed = false;

    while (1) {
        poll_inputs();

        unsigned event_flags = get_event_flags();
        if (event_flags & EV_ROCK_B_S) {
            digit_select++;
            if (digit_select >= N_DIGITS)
                digit_select = 0;
        }
        if (event_flags & EV_ROCK_A_S) {
            digit_select--;
            if (digit_select < 0)
                digit_select = N_DIGITS - 1;
        }
        if (event_flags & (EV_ROCK_A_S | EV_ROCK_B_S)) {
            digit_multiplier = 1;
            for (int i = 0; i < digit_select; i++)
                digit_multiplier *= 10;
        }

        int enc = get_encoder_ticks(true);
        if (enc != 0) {
            f_set += enc * digit_multiplier;
            f_set_changed = true;
        }

        // Reset all digits on the right of digit_select
        if (event_flags & EV_ROCK_SW_S) {
            f_set -= f_set % (digit_multiplier);
            f_set_changed = true;
        }

        if (f_set_changed) {
            // Enforce absolute frequency limits
            if (f_set < 12500000)
                f_set = 12500000;
            if (f_set > 6400000000)
                f_set = 6400000000;

            t_f_plan plan = {0};
            get_f_plan(f_set, &plan);
            print_f_plan(&plan);
            lmx_write_f_plan(&plan);
            f_set_changed = false;
        }

        fill(0);
        display_f_set();
        ssd1306_refresh();

        Delay_Ms(30);
    }
}
