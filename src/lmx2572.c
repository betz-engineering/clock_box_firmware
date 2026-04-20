#include "lmx2572.h"
#include <stdio.h>

#define CS_N(val) GPIO_WriteBit(GPIOA, PINA_LMX_CSB, val)

static const uint64_t F_PD = 64000000;  // Phase detector frequency when locked in [Hz]

// Helper function to exchange one byte
static uint8_t spi_rxtx(uint8_t byteToSend) {
    SPI_I2S_SendData(SPI1, byteToSend);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY))
        ;

    // Return the received data (and clear the RXNE flag)
    return SPI_I2S_ReceiveData(SPI1);
}

static void lmx_write_reg(uint8_t addr, uint16_t value) {
    CS_N(0);
    spi_rxtx(addr);
    spi_rxtx(value >> 8);
    spi_rxtx(value & 0xFF);
    CS_N(1);
}

static uint16_t lmx_read_reg(uint8_t addr) {
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

static int get_ch_div(uint64_t f_set) {
    int ch_div = 1;
    uint64_t f_vco = 6400000000 >> 1;
    while (ch_div < 256) {
        if (f_set >= f_vco)
            return ch_div;
        ch_div <<= 1;
        f_vco >>= 1;
    }
    return ch_div;
}

// Greatest common divisor from the Euclidean algorithm
static uint32_t calculate_gcd(uint32_t a, uint32_t b) {
    while (b != 0) {
        uint32_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void get_f_plan(uint64_t f_set, t_f_plan *plan) {
    int ch_div = get_ch_div(f_set);
    uint64_t f_vco = f_set * ch_div;
    int pll_n = f_vco / F_PD;
    uint32_t pll_num = f_vco - pll_n * F_PD;

    uint32_t gcd = calculate_gcd(pll_num, F_PD);

    plan->ch_div = ch_div;
    plan->pll_n = pll_n;
    plan->pll_num = pll_num / gcd;
    plan->pll_den = F_PD / gcd;
}

uint64_t get_f_out(t_f_plan *plan) {
    uint64_t f_vco = F_PD * plan->pll_n;
    f_vco += F_PD * plan->pll_num / plan->pll_den;
    return f_vco / plan->ch_div;
}

void print_f_plan(t_f_plan *plan) {
    printf(" ch_div: %d\n", plan->ch_div);
    printf("  pll_n: %d\n", plan->pll_n);
    printf("pll_num: %d\n", plan->pll_num);
    printf("pll_den: %d\n", plan->pll_den);
    printf("  f_out: %10llu Hz\n", get_f_out(plan));
}
