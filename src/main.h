#pragma once
#include <stdint.h>

#ifndef GIT_REV
#define GIT_REV "?"
#endif

#define TITLE_STR ("clock_box " GIT_REV)

#if (DEBUG)
#include <stdio.h>
#define D(format, ...) printf(format, ##__VA_ARGS__)
#else
#define D(X...)
#endif

// PORTA
#define PINA_LMX_SREF (1 << 1)
#define PINA_LMX_SYNC (1 << 2)
#define PINA_LMX_CE (1 << 3)
#define PINA_LMX_CSB (1 << 4)
#define PINA_LMX_SCK (1 << 5)
#define PINA_LMX_SDO (1 << 6)
#define PINA_LMX_SDI (1 << 7)
#define PINA_LMX_RCLK (1 << 9)
#define PINA_LMX_RDIR (1 << 15)

// PORTB
#define PINB_ENC_A (1 << 0)
#define PINB_ENC_B (1 << 1)
#define PINB_ENC_SW (1 << 3)
#define PINB_ROCK_A (1 << 4)
#define PINB_ROCK_B (1 << 5)
#define PINB_SCL (1 << 6)
#define PINB_SDA (1 << 7)
#define PINB_BOOT0 (1 << 8)

// PORTD
#define PIND_ROCK_SW (1 << 0)
