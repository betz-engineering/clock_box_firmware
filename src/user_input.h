#pragma once
#include <stdbool.h>

// Meaning of the bits in the value returned by get_button_flags()
// It indicates which events happened since the last call
#define EV_ROCK_A (1 << 0)
#define EV_ROCK_B (1 << 1)
#define EV_ROCK_SW (1 << 2)
#define EV_ENC_SW (1 << 3)

#define EV_ROCK_A_S (1 << 4)
#define EV_ROCK_B_S (1 << 5)
#define EV_ROCK_SW_S (1 << 6)
#define EV_ENC_SW_S (1 << 7)

#define EV_ROCK_A_L (1 << 8)
#define EV_ROCK_B_L (1 << 9)
#define EV_ROCK_SW_L (1 << 10)
#define EV_ENC_SW_L (1 << 11)

#define EV_ROT_CCW (1 << 12)
#define EV_ROT_CW (1 << 13)

void gpio_init(void);
void encoder_init(void);
void poll_inputs(void);

// if reset is true, returns number of encoder ticks (and direction) since last call
// if reset is false, returns accumulated encoder ticks
int get_encoder_ticks(bool reset);

// returns the instantaneous state of the encoder and back button (in the 2 LSBs)
// the other bits are used to indicate events. See the EV_ flags above.
unsigned get_event_flags(void);
