#pragma once

#include <stdint.h>

typedef struct {
    uint32_t ch_div;
    uint32_t pll_n;
    uint32_t pll_num;
    uint32_t pll_den;
} t_f_plan;


void lmx_init();

void lmx_dump();

void get_f_plan(uint64_t f_set, t_f_plan *plan);

uint64_t get_f_out(t_f_plan *plan);

void print_f_plan(t_f_plan *plan);
