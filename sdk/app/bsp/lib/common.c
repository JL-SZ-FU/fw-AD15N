#include "config.h"
#include "common.h"
#include "circular_buf.h"




AT_RAM
void delay(volatile u32 t)
{
    while (t--) {
        asm("nop");
    }
}

void system_enter_critical(void)
{
}

void system_exit_critical(void)
{
}

AT_RAM
u32 cbuf_write(cbuffer_t *cbuffer, void *buf, u32 len)
{
    extern u32 _cbuf_write(cbuffer_t *cbuffer, void *buf, u32 len);
    OS_ENTER_CRITICAL();
    u32 res = _cbuf_write(cbuffer, buf, len);
    OS_EXIT_CRITICAL();
    return res;
}


