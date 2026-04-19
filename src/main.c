#include "main.h"
#include "fixed.h"
#include "font.h"
#include "frame_buffer.h"
#include "graphics.h"
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
static int64_t f_set = 3222111000;
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

// Helper function to exchange one byte
static uint8_t spi_rxtx(uint8_t byteToSend) {
    SPI_I2S_SendData(SPI1, byteToSend);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY))
        ;

    // Return the received data (and clear the RXNE flag)
    return SPI_I2S_ReceiveData(SPI1);
}

#define CS_N(val) GPIO_WriteBit(GPIOA, PINA_LMX_CSB, val)

void lmx_write_reg(uint8_t addr, uint16_t value) {
    CS_N(0);
    spi_rxtx(addr);
    spi_rxtx(value >> 8);
    spi_rxtx(value & 0xFF);
    CS_N(1);
}

uint16_t lmx_read_reg(uint8_t addr) {
    CS_N(0);
    spi_rxtx(addr | 0x80);
    uint8_t val_h = spi_rxtx(0);
    uint8_t val_l = spi_rxtx(0);
    CS_N(1);
    return (val_h << 8) | val_l;
}

const static unsigned config_25MHz[] = {
    0x7D2288, 0x7C0000, 0x7B0000, 0x7A0000, 0x790000, 0x780000, 0x770000,
    0x760000, 0x750000, 0x740000, 0x730000, 0x727802, 0x710000, 0x700000,
    0x6F0000, 0x6E0000, 0x6D0000, 0x6C0000, 0x6B0000, 0x6A0007, 0x694440,
    0x682710, 0x670000, 0x660000, 0x650000, 0x642710, 0x630000, 0x620000,
    0x610000, 0x600000, 0x5F0000, 0x5E0000, 0x5D0000, 0x5C0000, 0x5B0000,
    0x5A0000, 0x590000, 0x580000, 0x570000, 0x560000, 0x55D800, 0x540001,
    0x530000, 0x522800, 0x510000, 0x50CCCC, 0x4F004C, 0x4E0107, 0x4D0000,
    0x4C000C, 0x4B0B00, 0x4A0000, 0x49003F, 0x480001, 0x470081, 0x46C350,
    0x450000, 0x4403E8, 0x430000, 0x4201F4, 0x410000, 0x401388, 0x3F0000,
    0x3E00AF, 0x3D00A8, 0x3C03E8, 0x3B0001, 0x3A9001, 0x390020, 0x380000,
    0x370000, 0x360000, 0x350000, 0x340421, 0x330080, 0x320080, 0x314180,
    0x3003E0, 0x2F0300, 0x2E07F0, 0x2DC61F, 0x2C1FA3, 0x2B0000, 0x2A0000,
    0x290000, 0x280000, 0x270001, 0x260000, 0x250205, 0x240032, 0x230004,
    0x220010, 0x211E01, 0x2005BF, 0x1FC3E6, 0x1E18A6, 0x1D0000, 0x1C0488,
    0x1B0002, 0x1A0808, 0x190624, 0x18071A, 0x17007C, 0x160001, 0x150409,
    0x144848, 0x1327B7, 0x120064, 0x11008A, 0x100080, 0x0F060E, 0x0E1820,
    0x0D4000, 0x0C5001, 0x0BB018, 0x0A10F8, 0x091004, 0x082000, 0x0700B2,
    0x06C802, 0x0530C8, 0x040A43, 0x030782, 0x020500, 0x010808, 0x00209C & ~(1 << 2),
};

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

    for (int i = 0; i < sizeof(config_25MHz) / sizeof(config_25MHz[0]); i++)
        lmx_write_reg(config_25MHz[i] >> 16, config_25MHz[i]);

    for (int i = 0; i < 128; i++)
        printf("R%03d: %04x\n", i, lmx_read_reg(i));

    ssd1306_i2c_init();
    Delay_Ms(10);  // Give the OLED some time to come up

    ssd1306_init();
    init_from_header(&f_profont);
    print_font_info();

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
        if (enc != 0)
            f_set += enc * digit_multiplier;

        // Reset all digits on the right of digit_select
        if (event_flags & EV_ROCK_SW_S)
            f_set -= f_set % (digit_multiplier);

        // Enforce absolute frequency limits
        if (f_set < 12500000)
            f_set = 12500000;
        if (f_set > 6400000000)
            f_set = 6400000000;

        fill(0);
        display_f_set();
        ssd1306_refresh();

        Delay_Ms(30);
    }
}
