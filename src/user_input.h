#pragma once
#include <stdbool.h>
#include <stdint.h>

// Meaning of the bits in the value returned by get_button_flags()
// It indicates which events happened since the last call
// Instantaneous value
#define EV_ROCK_A (1 << 0)
#define EV_ROCK_B (1 << 1)
#define EV_ROCK_SW (1 << 2)
#define EV_ENC_SW (1 << 3)

// On push
#define EV_ROCK_A_P (1 << 4)
#define EV_ROCK_B_P (1 << 5)
#define EV_ROCK_SW_P (1 << 6)
#define EV_ENC_SW_P (1 << 7)

// Short push and release
#define EV_ROCK_A_S (1 << 8)
#define EV_ROCK_B_S (1 << 9)
#define EV_ROCK_SW_S (1 << 10)
#define EV_ENC_SW_S (1 << 11)

// Long push and release
#define EV_ROCK_A_L (1 << 12)
#define EV_ROCK_B_L (1 << 13)
#define EV_ROCK_SW_L (1 << 14)
#define EV_ENC_SW_L (1 << 15)

// Encoder tick
#define EV_ROT_CCW (1 << 16)
#define EV_ROT_CW (1 << 17)

#define CS_N(val) GPIO_WriteBit(GPIOA, PINA_LMX_CSB, val)

void peripherals_init(void);
void encoder_init(void);
void poll_inputs(void);

// if reset is true, returns number of encoder ticks (and direction) since last call
// if reset is false, returns accumulated encoder ticks
int get_encoder_ticks(bool reset);

// returns the instantaneous state of the encoder and back button (in the 2 LSBs)
// the other bits are used to indicate events. See the EV_ flags above.
unsigned get_event_flags(void);

// Send (and receive) one byte on the SPI. Blocking.
uint8_t spi_rxtx(uint8_t val);

// Return a running number of time since boot in [ms]
unsigned millis(void);

// Blocking wait in [ms]
// use this instead of Delay_Ms() from debug.h, as the latter will mess with the
// systick-timer hardware configuration.
void delay_ms(unsigned val);

// Setup the cycles / systick interrupt
void sys_tick_config(uint32_t ticks);
