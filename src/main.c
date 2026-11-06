// SWO / ITM test for STM32F4DISCOVERY (STM32F407VG).
//  - ITM port 0: printf() text, once per second
//  - ITM port 1: UTF-8 text via swo_puts()
//  - LEDs LD4 green, LD3 orange, LD5 red, LD6 blue (PD12..PD15) spin around so you can see it's alive
// SWO pin is PB3, wired to the on-board ST-LINK/V2 (solder bridge SB12, closed by default).
#include <stdio.h>
#include "swo.h"

#define REG(addr) (*(volatile uint32_t *)(addr))

#define RCC_AHB1ENR   REG(0x40023830)
#define GPIOD_MODER   REG(0x40020C00)
#define GPIOD_ODR     REG(0x40020C14)

#define SYST_CSR      REG(0xE000E010)
#define SYST_RVR      REG(0xE000E014)
#define SYST_CVR      REG(0xE000E018)

static volatile uint32_t g_ticks;

void SysTick_Handler(void) { g_ticks++; }

static void delay_ms(uint32_t ms)
{
    uint32_t start = g_ticks;
    while ((g_ticks - start) < ms) {}
}

int main(void)
{
    // 1 ms SysTick from the 16 MHz core clock
    SYST_RVR = SWO_CPU_HZ / 1000u - 1u;
    SYST_CVR = 0;
    SYST_CSR = 0x7;  // CLKSOURCE = core, TICKINT, ENABLE

    // PD12..PD15 as outputs
    RCC_AHB1ENR |= (1u << 3);  // GPIODEN
    (void)RCC_AHB1ENR;         // let the clock settle before touching GPIOD
    GPIOD_MODER = (GPIOD_MODER & 0x00FFFFFFu) | 0x55000000u;

    swo_init();
    setvbuf(stdout, NULL, _IONBF, 0);  // no buffering: each printf goes out right away

    printf("\nSWO test started on STM32F407VG: core %lu Hz, SWO %lu baud\n",
           (unsigned long)SWO_CPU_HZ, (unsigned long)SWO_BAUD);

    uint32_t counter = 0;
    for (;;) {
        printf("[port0] tick %lu, uptime %lu ms\n",
               (unsigned long)counter, (unsigned long)g_ticks);
        if ((counter % 5u) == 0)
            swo_puts(1, "[port1] Привет, SWO! ✓\n");

        for (int i = 0; i < 4; i++) {
            GPIOD_ODR = (GPIOD_ODR & 0x0FFFu) | (1u << (12 + i));
            delay_ms(250);
        }
        counter++;
    }
}
