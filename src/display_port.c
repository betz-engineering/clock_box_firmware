#include <ch32fun.h>
#include <ssd1306_i2c.h>
#include <stdio.h>
#include <u8g2.h>

#define TINY_TO_NS ((1000000000 / FUNCONF_SYSTEM_CORE_CLOCK) * 6)

uint8_t u8g2_gpio_and_delay_ch32fun(U8X8_UNUSED u8x8_t *u8x8,
                                    U8X8_UNUSED uint8_t msg,
                                    U8X8_UNUSED uint8_t arg_int,
                                    U8X8_UNUSED void *arg_ptr) {
    switch (msg) {
    case U8X8_MSG_DELAY_NANO:     // delay arg_int * 100 nano seconds
    case U8X8_MSG_DELAY_100NANO:  // delay arg_int * 100 nano seconds
        break;
    case U8X8_MSG_DELAY_10MICRO:  // delay arg_int * 10 micro seconds
        Delay_Us(10);
        break;
    case U8X8_MSG_DELAY_MILLI:  // delay arg_int * 1 milli second
        Delay_Ms(arg_int);
        break;
    default:
        return 0;
    }

    return 1;
}

uint8_t u8x8_byte_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    static uint8_t buffer[32];  // u8g2/u8x8 will never send more than 32 bytes between
                                // START_TRANSFER and END_TRANSFER
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg) {
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0) {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
        break;
    case U8X8_MSG_BYTE_INIT:
        /* add your custom code to init i2c subsystem */
        break;
    case U8X8_MSG_BYTE_SET_DC:
        /* ignored for i2c */
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        ssd1306_i2c_send(u8x8_GetI2CAddress(u8x8) >> 1, buffer, buf_idx);
        break;
    default:
        return 0;
    }
    return 1;
}

void display_init(u8g2_t *u8g2) {
    ssd1306_i2c_init();
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
        u8g2, U8G2_R0, u8x8_byte_i2c, u8g2_gpio_and_delay_ch32fun);
    u8g2_InitDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);
}
