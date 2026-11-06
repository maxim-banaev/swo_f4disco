#include "swo.h"

#define REG(addr) (*(volatile uint32_t *)(addr))

#define DEMCR        REG(0xE000EDFC)
#define DBGMCU_CR    REG(0xE0042004)
#define TPIU_ACPR    REG(0xE0040010)
#define TPIU_SPPR    REG(0xE00400F0)
#define TPIU_FFCR    REG(0xE0040304)
#define ITM_STIM(n)  REG(0xE0000000u + 4u * (n))
#define ITM_TER      REG(0xE0000E00)
#define ITM_TPR      REG(0xE0000E40)
#define ITM_TCR      REG(0xE0000E80)
#define ITM_LAR      REG(0xE0000FB0)

void swo_init(void)
{
    // The debugger (Cortex-Debug swoConfig) also programs these registers;
    // doing it here too makes SWO work with any probe/tool, e.g. after a plain reset.
    DEMCR |= (1u << 24);                  // TRCENA: enable DWT/ITM/TPIU
    DBGMCU_CR = (DBGMCU_CR & ~(3u << 6))  // TRACE_MODE = 00: asynchronous (SWO on PB3)
              | (1u << 5);                // TRACE_IOEN

    TPIU_SPPR = 2;                        // NRZ (UART) encoding
    TPIU_ACPR = SWO_CPU_HZ / SWO_BAUD - 1;
    TPIU_FFCR = 0x100;                    // TrigIn on, formatter off

    ITM_LAR = 0xC5ACCE55;                 // unlock ITM
    ITM_TCR = (1u << 16)                  // ATB ID 1
            | (1u << 3)                   // DWTENA
            | (1u << 2)                   // SYNCENA
            | (1u << 0);                  // ITMENA
    ITM_TPR = 0;                          // all ports accessible from unprivileged code
    ITM_TER = 0x3;                        // enable stimulus ports 0 and 1
}

void swo_putc(uint8_t port, char c)
{
    if (!(DEMCR & (1u << 24)) || !(ITM_TCR & 1u) || !(ITM_TER & (1u << port)))
        return;
    while ((ITM_STIM(port) & 1u) == 0) {}  // wait for FIFO ready
    *(volatile uint8_t *)&ITM_STIM(port) = (uint8_t)c;
}

void swo_puts(uint8_t port, const char *s)
{
    while (*s) swo_putc(port, *s++);
}

// newlib retarget: printf/puts -> ITM port 0
int _write(int fd, const char *buf, int len)
{
    (void)fd;
    for (int i = 0; i < len; i++) swo_putc(0, buf[i]);
    return len;
}
