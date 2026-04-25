#include "lmx2572.h"
#include "main.h"
#include "user_input.h"
#include <ch32v20x.h>
#include <stdint.h>
#include <stdio.h>

#define MASH_ORDER 3

const static unsigned config_25MHz[] = {
    0x727802, 0x682710, 0x642710, 0x5F0000, 0x55D800,
    0x540001, 0x522800, 0x50CCCC, 0x4F004C, 0x4E0107,
    0x4C000C, 0x4B0B00, 0x470081,
    0x3EFCAF,  // 0xFC: enable all double buffering
    0x3A9001, 0x390020, 0x340421, 0x2DC61F, 0x2C1FE0 | MASH_ORDER,
    0x270001, 0x260000, 0x240032, 0x1E18A6, 0x1D0000,
    0x144848, 0x11008A, 0x0E1820, 0x091004, 0x0538C8,
    0x00209C,
};

static const uint64_t F_PD = 64000000;         // Phase detector frequency when locked in [Hz]
static const uint64_t F_VCO_MAX = 6400000000;  // Max. frequency limit of the VCO
static const uint64_t F_VCO_MIN = 3200000000;  // Min. frequency limit of the VCO

// These get overwritten with the values from config_25MHz
static uint16_t config_r0 = 0, config_r44 = 0x22a2, config_r45 = 0xc622;

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

void lmx_init() {
    lmx_write_reg(0, 2);  // Reset all registers to initial values

    // write initial config
    for (int i = 0; i < sizeof(config_25MHz) / sizeof(config_25MHz[0]); i++) {
        uint8_t adr = config_25MHz[i] >> 16;
        uint16_t val = config_25MHz[i];
        if (adr == 0) {
            val &= ~(1 << 2);  // reset MUXOUT_LD_SEL, enable register readback
            config_r0 = val;
        } else if (adr == 44) {
            config_r44 = val;
        } else if (adr == 45) {
            config_r45 = val;
        }
        lmx_write_reg(adr, val);
    }
}

void lmx_dump() {
    for (int i = 0; i < 128; i++)
        D("R%03d: %04x\n", i, lmx_read_reg(i));
}

// Return the output channel divider value to get the
// VCO frequency close to the max. limit
static int get_ch_div(uint64_t f_set) {
    int ch_div = 1;
    uint64_t f_vco = F_VCO_MAX >> 1;
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

bool get_f_plan(uint64_t f_set, t_f_plan *plan) {
    int ch_div = get_ch_div(f_set);
    if (ch_div <= 0)
        return false;

    uint64_t f_vco = f_set * ch_div;
    if (f_vco > F_VCO_MAX || f_vco < F_VCO_MIN)
        return false;

    int pll_n = f_vco / F_PD;
    uint32_t pll_num = f_vco - pll_n * F_PD;

    uint32_t gcd = calculate_gcd(pll_num, F_PD);

    plan->ch_div = ch_div;
    plan->pll_n = pll_n;
    plan->pll_num = pll_num / gcd;
    plan->pll_den = F_PD / gcd;
    plan->f_vco = f_vco;
    plan->f_out = f_set;
    return true;
}

void print_f_plan(t_f_plan *plan) {
    // horrible hack because the included printf doesn't support 64 bit int
    D("  f_out: %u%u Hz\n", (unsigned)(plan->f_out / 10000), (unsigned)(plan->f_out % 10000));
    D("  f_vco: %u%u Hz\n", (unsigned)(plan->f_vco / 10000), (unsigned)(plan->f_vco % 10000));
    D(" ch_div: %ld\n", plan->ch_div);
    D("  pll_n: %ld\n", plan->pll_n);
    D("pll_num: %ld\n", plan->pll_num);
    D("pll_den: %ld\n\n", plan->pll_den);
}

void lmx_set_output_enable(bool en_a, bool en_b) {
    config_r44 |= (1 << 6) | (1 << 7);
    if (en_a)
        config_r44 &= ~(1 << 6);  // reset OUTA_PD
    if (en_b)
        config_r44 &= ~(1 << 7);  // reset OUTB_PD
    lmx_write_reg(44, config_r44);
}

void lmx_set_outa_pwr(int val) {
    config_r44 &= ~0x3F00;
    config_r44 |= (val & 0x3F) << 8;
    lmx_write_reg(44, config_r44);
}

void lmx_set_outb_pwr(int val) {
    config_r45 &= ~0x003F;
    config_r45 |= val & 0x3F;
    lmx_write_reg(45, config_r45);
}

static void lmx_set_ch_div(int divider) {
    // ch_div is a frequency division factor within 1 to 256
    // only the divider = 2^x values are valid

    // Set the OUTA_MUX. 0 = use channel divider
    config_r45 &= ~((1 << 11) | (1 << 12));
    if (divider == 1)
        config_r45 |= (1 << 11);  // directly use VCO
    lmx_write_reg(45, config_r45);

    // we don't use the divider and we are done.
    if (divider == 1)
        return;

    // Set the channel divider value
    unsigned code = 14;
    if (divider == 2)
        code = 0;
    else if (divider == 4)
        code = 1;
    else if (divider == 8)
        code = 3;
    else if (divider == 16)
        code = 5;
    else if (divider == 32)
        code = 7;
    else if (divider == 64)
        code = 9;
    else if (divider == 128)
        code = 12;
    else if (divider == 256)
        code = 14;
    else
        D("Error: Illegal output divider value: %d. Falling back to 256.\n", divider);

    lmx_write_reg(75, (1 << 11) | (code << 6));
}

void lmx_write_f_plan(t_f_plan *plan) {
    lmx_set_ch_div(plan->ch_div);
    lmx_write_reg(43, plan->pll_num);
    lmx_write_reg(42, plan->pll_num >> 16);
    lmx_write_reg(39, plan->pll_den);
    lmx_write_reg(38, plan->pll_den >> 16);

    // Adjust PH_DLY_SEL according to the N divider
    unsigned pfd_dly_sel;
#if MASH_ORDER == 0
    pfd_dly_sel = (plan->f_vco < 4000000000) ? 0 : 1;
#elif MASH_ORDER == 1
    pfd_dly_sel = (plan->f_vco < 4000000000) ? 1 : 2;
#elif MASH_ORDER == 2
    pfd_dly_sel = (plan->f_vco < 4000000000) ? 1 : 2;
#elif MASH_ORDER == 3
    pfd_dly_sel = (plan->f_vco < 4900000000) ? 2 : 3;
#elif MASH_ORDER == 4
    pfd_dly_sel = (plan->f_vco < 4900000000) ? 4 : 5;
#endif
    lmx_write_reg(37, (pfd_dly_sel << 8) | 5);

    lmx_write_reg(36, plan->pll_n);
    lmx_write_reg(34, 0x0010 | ((plan->pll_n >> 16) & 7));

    // Set FCAL_EN to latch the double-buffered values
    config_r0 |= (1 << 3);
    lmx_write_reg(0, config_r0);
}
