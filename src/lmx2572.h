#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t ch_div;
    uint32_t pll_n;
    uint32_t pll_num;
    uint32_t pll_den;
    uint64_t f_vco;
    uint64_t f_out;
} t_f_plan;

void lmx_init();

void lmx_dump();

void get_f_plan(uint64_t f_set, t_f_plan *plan);

uint64_t get_f_out(t_f_plan *plan);

void print_f_plan(t_f_plan *plan);

void lmx_write_f_plan(t_f_plan *plan);

void lmx_set_outa_pwr(int val);
void lmx_set_outb_pwr(int val);

void lmx_set_output_enable(bool en_a, bool en_b);
