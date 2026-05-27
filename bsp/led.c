#include "led.h"

void led_init(void)
{
    volatile uint32_t *pRccAhbEnr =
            (uint32_t*)0x40021014;

    volatile uint32_t *pGpioBMode =
            (uint32_t*)0x48000400;

    // GPIOB clock enable
    *pRccAhbEnr |= (1U<<18);

    // PB3 output mode
    *pGpioBMode &= ~(3U<<(2*LED_GREEN));
    *pGpioBMode |=  (1U<<(2*LED_GREEN));
}

void led_on(void)
{
    volatile uint32_t *pOdr =
            (uint32_t*)0x48000414;

    *pOdr |= (1U<<LED_GREEN);
}

void led_off(void)
{
    volatile uint32_t *pOdr =
            (uint32_t*)0x48000414;

    *pOdr &= ~(1U<<LED_GREEN);
}
