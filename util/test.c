#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    uint32_t ch_div;
    uint32_t pll_n;
    uint32_t pll_num;
    uint32_t pll_den;
} t_f_plan;

static const uint64_t F_PD = 64000000;  // Phase detector frequency when locked in [Hz]

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

int main() {
    t_f_plan plan = {0};

    srandom(time(NULL));

    for (unsigned i = 0; i < 100000000; i++) {
        uint64_t rnd64 = random() | ((uint64_t)(random()) << 32);
        rnd64 %= (6400000000 - 12500000);
        uint64_t f_set = 12500000 + rnd64;
        if (i == 0)
            f_set = 1234567890;
        get_f_plan(f_set, &plan);

        if (f_set != get_f_out(&plan)) {
            printf("error!!!! f_set = %10llu\n", f_set);
            print_f_plan(&plan);
        }
        if (i % 10000000 == 0) {
            printf("\n%d\n", i);
            print_f_plan(&plan);
        }
    }
}
