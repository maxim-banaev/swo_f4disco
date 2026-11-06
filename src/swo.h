#pragma once
#include <stdint.h>

// Core clock after reset = HSI 16 MHz (no PLL set up). Must match "cpuFrequency" in the debug profile.
#define SWO_CPU_HZ   16000000u
// Must match "swoFrequency" in the debug profile. 16 MHz / 2 MHz = integer prescaler, ST-LINK/V2 handles 2 MHz.
#define SWO_BAUD     2000000u

void swo_init(void);
// Send one byte on ITM stimulus port 'port' (0..31). No-op if no debugger enabled the port.
void swo_putc(uint8_t port, char c);
void swo_puts(uint8_t port, const char *s);
