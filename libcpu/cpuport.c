/*
 * File      : cpu.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-01-13     weety      modified from mini2440
 * 2015-04-15     ArdaFu     Add code for IAR
 */

#include <rthw.h>
#include <rtthread.h>

#define ICACHE_MASK    (rt_uint32_t)(1 << 12)
#define DCACHE_MASK    (rt_uint32_t)(1 << 2)

extern void machine_reset(void);
extern void machine_shutdown(void);


#if defined(__CC_ARM)
rt_inline rt_uint32_t cp15_rd(void)
{
    rt_uint32_t i;

    __asm volatile
    {
        mrc p15, 0, i, c1, c0, 0
    }

    return i;
}

rt_inline void cache_enable(rt_uint32_t bit)
{
    rt_uint32_t value;

    __asm volatile
    {
        mrc p15, 0, value, c1, c0, 0
        orr value, value, bit
        mcr p15, 0, value, c1, c0, 0
    }
}

rt_inline void cache_disable(rt_uint32_t bit)
{
    rt_uint32_t value;

    __asm volatile
    {
        mrc p15, 0, value, c1, c0, 0
        bic value, value, bit
        mcr p15, 0, value, c1, c0, 0
    }
}
#endif

/**
 * enable I-Cache
 *
 */
void rt_hw_cpu_icache_enable()
{
    cache_enable(ICACHE_MASK);
}

/**
 * disable I-Cache
 *
 */
void rt_hw_cpu_icache_disable()
{
    cache_disable(ICACHE_MASK);
}

/**
 * return the status of I-Cache
 *
 */
rt_base_t rt_hw_cpu_icache_status()
{
    return (cp15_rd() & ICACHE_MASK);
}

/**
 * enable D-Cache
 *
 */
void rt_hw_cpu_dcache_enable()
{
    cache_enable(DCACHE_MASK);
}

/**
 * disable D-Cache
 *
 */
void rt_hw_cpu_dcache_disable()
{
    cache_disable(DCACHE_MASK);
}

/**
 * return the status of D-Cache
 *
 */
rt_base_t rt_hw_cpu_dcache_status()
{
    return (cp15_rd() & DCACHE_MASK);
}

/**
 * reset cpu by dog's time-out
 *
 */
void rt_hw_cpu_reset()
{

    rt_kprintf("Restarting system...\n");
    machine_reset();

    while(1);    /* loop forever and wait for reset to happen */

    /* NEVER REACHED */
}

/**
 *  shutdown CPU
 *
 */
void rt_hw_cpu_shutdown()
{
    rt_uint32_t level;
    rt_kprintf("shutdown...\n");

    level = rt_hw_interrupt_disable();
    machine_shutdown();
    while (level)
    {
        RT_ASSERT(0);
    }
}

#ifdef RT_USING_CPU_FFS
/**
 * This function finds the first bit set (beginning with the least significant bit)
 * in value and return the index of that bit.
 *
 * Bits are numbered starting at 1 (the least significant bit).  A return value of
 * zero from any of these functions means that the argument was zero.
 *
 * @return return the index of the first bit set. If value is 0, then this function
 * shall return 0.
 */
#if defined(__CC_ARM)
int __rt_ffs(int value)
{
    register rt_uint32_t x;

    if (value == 0)
        return value;

    __asm
    {
        rsb x, value, #0
        and x, x, value
        clz x, x
        rsb x, x, #32
    }

    return x;
}
#elif defined(__GNUC__) || defined(__ICCARM__)
int __rt_ffs(int value)
{
    register rt_uint32_t x;

    if (value == 0)
        return value;

    __asm
    (
        "rsb %[temp], %[val], #0\n"
        "and %[temp], %[temp], %[val]\n"
        "clz %[temp], %[temp]\n"
        "rsb %[temp], %[temp], #32\n"
        :[temp] "=r"(x)
        :[val] "r"(value)
    );
    return x;
}
#endif

#endif


/*@}*/
