#pragma once

#include <stdbool.h>
#include <stdint.h>

// All divider values needed to generate a specific frequency
typedef struct {
    uint32_t ch_div;
    uint32_t pll_n;
    uint32_t pll_num;
    uint32_t pll_den;
    uint64_t f_vco;
    uint64_t f_out;
} t_f_plan;

// Send initial config generated with TI tool for 25 MHz with outputs disabled
void lmx_init();

// Dump all register values from 0 - 128 to stdout
void lmx_dump();

// Calculate divider values for a given frequency set-point.
// returns true on success and writes results to plan.
bool get_f_plan(uint64_t f_set, t_f_plan *plan);

// print all values of a plan to stdout
void print_f_plan(t_f_plan *plan);

// Set frequency on the LMX according to the specified plan
void lmx_write_f_plan(t_f_plan *plan);

// Set output power on the LMX. Directly writes val to OUTx_PWR
// Check Figure 166 in LMX2572 datasheet for mapping to output power.
void lmx_set_outa_pwr(int val);
void lmx_set_outb_pwr(int val);

// Enable / disable the RF
void lmx_set_output_enable(bool en_a, bool en_b);
