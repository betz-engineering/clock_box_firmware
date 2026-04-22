#include "user_input.h"
#include "main.h"
#include <ch32v20x.h>
#include <ch32v20x_gpio.h>
#include <stdio.h>

#define T_LONG 500  // time for a long button push in [ms]

static unsigned event_flags = 0;
static volatile int enc_sum = 0;

// -----------------------------
//  Systick interrupt
// -----------------------------
static volatile uint32_t system_ticks = 0;

__attribute__((interrupt)) void SysTick_Handler(void) {
    SysTick->SR = 0;
    system_ticks++;
}

void sys_tick_config(uint32_t ticks) {
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->CTLR = 0;
    SysTick->SR = 0;
    SysTick->CNT = 0;
    SysTick->CMP = ticks - 1;
    SysTick->CTLR = 0xF;
}

unsigned millis(void) { return system_ticks; }

void delay_ms(unsigned val) {
    unsigned t2 = millis() + val;
    while (millis() < t2)
        ;
}

int get_encoder_ticks(bool reset) {
    static int last_ticks = 0;
    int tmp = enc_sum;

    int ret = tmp - last_ticks;

    if (reset)
        last_ticks = tmp;

    return ret;
}

// Converts the encoder number into a series of left / right events
static unsigned get_rotate_events(void) {
    static int processed_ticks = 0;
    unsigned flags = 0;
    int ticks = get_encoder_ticks(false);

    if (ticks < processed_ticks) {
        flags |= EV_ROT_CCW;
        processed_ticks--;
    } else if (ticks > processed_ticks) {
        flags |= EV_ROT_CW;
        processed_ticks++;
    }
    return flags;
}

unsigned get_event_flags(void) {
    unsigned ret = event_flags;
    event_flags &= 0xF;  // clear the sticky button-push event flags
    ret |= get_rotate_events();
    return ret;
}

void encoder_init(void) {
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    // Route EXTI lines to PB0 and PB1 using AFIO
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

    // Configure EXTI lines for rising edge only
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Configure the NVIC for EXTI0 (PB0)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  // lowest
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// CH32V specific attribute for fast hardware context saving
void __attribute__((interrupt("WCH-Interrupt-fast"))) EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        // ENC_A just had a rising edge, so ENC_B must be stable now
        // We assume excellent analog debouncing here. Otherwise we would need to
        // introduce some dead-time here.
        enc_sum += GPIO_ReadInputDataBit(GPIOB, PINB_ENC_B) ? 1 : -1;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void peripherals_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD |
                               RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1,
                           ENABLE);
    // --------
    //  GPIOs
    // --------
    // Port A outputs
    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_10MHz;
    gpio.GPIO_Pin =
        PINA_LMX_SREF | PINA_LMX_SYNC | PINA_LMX_CE | PINA_LMX_RCLK | PINA_LMX_RDIR | PINA_LMX_CSB;
    // Enable the LMX and de-select the SPI
    GPIO_Write(GPIOA, PINA_LMX_CE | PINA_LMX_CSB);
    GPIO_Init(GPIOA, &gpio);

    // Port A alternate-function outputs
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = PINA_LMX_SCK | PINA_LMX_SDI;
    gpio.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &gpio);

    // Port B alternate-function outputs
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Pin = PINB_SCL | PINB_SDA;
    GPIO_Init(GPIOB, &gpio);

    // Port B inputs
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = PINB_ROCK_A | PINB_ROCK_B | PINB_ENC_A | PINB_ENC_B | PINB_ENC_SW;
    GPIO_Init(GPIOB, &gpio);

    // Remap OSC pins to GPIO
    GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);

    // Port D inputs
    gpio.GPIO_Pin = PIND_ROCK_SW;
    GPIO_Init(GPIOD, &gpio);

    // --------
    //  SPI
    // --------
    SPI_InitTypeDef spi_cfg = {0};
    SPI_StructInit(&spi_cfg);
    spi_cfg.SPI_Mode = SPI_Mode_Master;
    spi_cfg.SPI_DataSize = SPI_DataSize_8b;
    spi_cfg.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;  // 9 MHz I think
    spi_cfg.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI1, &spi_cfg);
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t spi_rxtx(uint8_t val) {
    SPI_I2S_SendData(SPI1, val);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY))
        ;

    // Return the received data (and clear the RXNE flag)
    return SPI_I2S_ReceiveData(SPI1);
}

void poll_inputs() {
    static unsigned push_cycles[4], last_buttons = 0;

    // Read clock cycle counter
    unsigned cycles = millis();

    // Read the IO pin state
    uint16_t pb_val = GPIO_ReadInputData(GPIOB);
    uint16_t pd_val = GPIO_ReadInputData(GPIOD);

    // printf("ENC: %d %d %d   SW: %d %d %d\n",
    //        (pb_val & PINB_ENC_B) > 0,
    //        (pb_val & PINB_ENC_A) > 0,
    //        (pb_val & PINB_ENC_SW) > 0,
    //        (pb_val & PINB_ROCK_A) > 0,
    //        (pb_val & PINB_ROCK_B) > 0,
    //        (pd_val & PIND_ROCK_SW) > 0);

    unsigned buttons = 0;
    if ((pb_val & PINB_ROCK_A) == 0)
        buttons |= 1;
    if ((pb_val & PINB_ROCK_B) == 0)
        buttons |= 2;
    if ((pd_val & PIND_ROCK_SW) == 0)
        buttons |= 4;
    if ((pb_val & PINB_ENC_SW) == 0)
        buttons |= 8;

    // Set instantaneous button state in bits 0x00F
    event_flags &= ~0xF;
    event_flags |= buttons;

    // decode buttons states (rising or falling edge of the input signal)
    unsigned rising = (~last_buttons) & buttons;
    unsigned falling = last_buttons & (~buttons);
    last_buttons = buttons;

    for (int i = 0; i < 4; i++) {
        if (rising & (1 << i)) {
            // On button push, latch the current cycle count
            event_flags |= EV_ROCK_A_P << i;
            push_cycles[i] = cycles;
        } else if (push_cycles[i] > 0) {
            if (falling & (1 << i)) {
                // On button release, fire a short-press event
                event_flags |= EV_ROCK_A_S << i;
                push_cycles[i] = 0;
            } else if ((cycles - push_cycles[i]) >= T_LONG) {
                // On timeout, fire a long-press event
                event_flags |= EV_ROCK_A_L << i;
                push_cycles[i] = 0;
            }
        }
    }

    // static unsigned last_event_flags = 0;
    // if (event_flags != last_event_flags) {
    //     printf("event_flags: %04x\n", event_flags);
    //     last_event_flags = event_flags;
    // }
}
