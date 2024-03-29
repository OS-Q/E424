#include <interface.h>

uint16_t convert_byte_array_to_16_bit(uint8_t *data)
{
    return (data[0] | ((uint16_t)data[1] << 8));
}

uint32_t convert_byte_array_to_32_bit(uint8_t *data)
{
    union {
        uint32_t u32;
        uint8_t u8[4];
    } long_addr;
    uint8_t index;
    for (index = 0; index < 4; index++)
        long_addr.u8[index] = *data++;
    return long_addr.u32;
}

void gclk_setup(int generator, uint32_t reg)
{
    while (gclk_is_syncing(generator))
    {
    }
    GCLK->GENCTRL[generator].reg = reg;
    while (gclk_is_syncing(generator))
    {
    }
}

void gclk_channel_setup(const uint8_t channel, uint32_t val)
{
    GCLK->PCHCTRL[channel].reg = val;
    while (0 == (GCLK->PCHCTRL[channel].reg & GCLK_PCHCTRL_CHEN))
    {
    }
}

void gclk_channel_enable(const uint8_t channel)
{
    __disable_irq();
    GCLK->PCHCTRL[channel].reg |= GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[channel].reg & GCLK_PCHCTRL_CHEN)
    {
    }
    __enable_irq();
}

void gclk_channel_disable(const uint8_t channel)
{
    __disable_irq();
    GCLK->PCHCTRL[channel].reg &= ~GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[channel].reg & GCLK_PCHCTRL_CHEN)
    {
    }
    __enable_irq();
}

uint32_t rnd(void)
{
    TRNG->CTRLA.reg |= TRNG_CTRLA_ENABLE;
    while (((TRNG->INTFLAG.reg) & (TRNG_INTFLAG_DATARDY)) != TRNG_INTFLAG_DATARDY)
    {
    }
    TRNG->CTRLA.reg &= ~(TRNG_CTRLA_ENABLE);
    return (TRNG->DATA.reg);
}

void kick_watchdog(void)
{
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
    while (WDT->SYNCBUSY.reg)
    {
    }
}

int serial_number(uint8_t *sn, uint32_t size)
{
    if (sn && size == 16)
    {
        memset(sn, 0, 16);
        uint32_t *p = (uint32_t *)sn;
        *p++ = *((uint32_t *)0x80A00C);
        *p++ = *((uint32_t *)0x80A040);
        *p++ = *((uint32_t *)0x80A044);
        *p++ = *((uint32_t *)0x80A048);
        return 0;
    }
    return -1;
}

// Debug helper for C sources

#if DEBUG_ENABLE > 0

#define DBG_UART SERCOM0

char DBG_BUFFER[DBG_BUF_LEN];

void putChar(char c)
{
    if (DBG_UART->USART.CTRLA.bit.ENABLE == 0)
        return;
    while (DBG_UART->USART.INTFLAG.bit.DRE == 0)
    {
    }
    DBG_UART->USART.DATA.reg = c;
}

void putString(char *str)
{
    while (*str)
        putChar(*str++);
}

#endif