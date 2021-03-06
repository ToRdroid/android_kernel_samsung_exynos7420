/* linux/drivers/cpufreq/pmu_func.h
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PMU_FUNC_H_
#define __PMU_FUNC_H_ __FILE__

#if defined(CONFIG_ARM64)
static inline void pmu_enable_counter(u32 val)
{
	val = 1 << val;
	asm volatile("msr PMCNTENSET_EL0, %0" : : "r" (val));
}

static inline void pmu_disable_counter(u32 val)
{
	val = 1 << val;
	asm volatile("msr PMCNTENCLR_EL0, %0" : : "r" (val));
}

static inline void pmnc_config(u32 counter, u32 event)
{
	counter = counter & 0x1F;
	asm volatile("msr PMSELR_EL0, %0" : : "r" (counter));
	asm volatile("msr PMXEVTYPER_EL0, %0" : : "r" (event));
}

static inline void pmnc_reset(void)
{
	u32 val;
	asm volatile("mrs %0, PMCR_EL0" : "=r"(val));
	val = val | 0x2;
	asm volatile("msr PMCR_EL0, %0" : : "r"(val));
}

static inline u32 pmnc_read2(u32 counter)
{
	u32 ret;
	counter = counter & 0x1F;
	asm volatile("msr PMSELR_EL0, %0" : : "r" (counter));
	asm volatile("mrs %0, PMXEVCNTR_EL0" : "=r"(ret));
	return ret;
}

static inline u32 pmnc_read_evt(u32 counter)
{
	u32 ret;
	counter = counter & 0x1F;
	asm volatile("msr PMSELR_EL0, %0" : : "r" (counter));
	asm volatile("mrs %0, PMXEVTYPER_EL0" : "=r"(ret));
	return ret;
}

static inline void pmnc_write(u32 counter, u32 val)
{
	counter = counter & 0x1F;
	asm volatile("msr PMSELR_EL0, %0" : : "r" (counter));
	asm volatile("msr PMXEVCNTR_EL0, %0" : : "r" (val));
}

static inline void enable_pmu(void)
{
	u32 val;

	asm volatile("mrs %0, PMCR_EL0" : "=r"(val));
	val = val | 0x1;	// set E bit
	asm volatile("msr PMCR_EL0, %0" : : "r"(val));
}

static inline void disable_pmu(void)
{
	u32 val;
	asm volatile("mrs %0, PMCR_EL0" : "=r"(val));
	val = val & ~0x1;	// clear E bit
	asm volatile("msr PMCR_EL0, %0" : : "r"(val));
}

static inline u32 read_pmnc(void)
{
	u32 val;
	asm volatile("mrs %0, PMCR_EL0" : "=r"(val));
	return val;
}

static inline void disable_ccnt(void)
{
	u32 val = 0x80000000;
	asm volatile("msr PMCNTENCLR_EL0, %0" : : "r" (val));
}

static inline void enable_ccnt(void)
{
	u32 val = 0x80000000;
	asm volatile("msr PMCNTENSET_EL0, %0" : : "r" (val));
}

static inline void reset_ccnt(void)
{
	u32 val;
	asm volatile("mrs %0, PMCR_EL0" : "=r"(val));
	val = val | 0x4;	// set C bit
	asm volatile("msr PMCR_EL0, %0" : : "r"(val));
}

static inline u64 read_ccnt(void)
{
	u64 val;
	asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(val));
	return val;
}

static inline void write_ccnt(u64 val)
{
	asm volatile("msr PMCCNTR_EL0, %0" : : "r"(val));
}
#else
static inline void pmu_enable_counter(u32 val)
{
	val = 1 << val;
	asm volatile("mcr p15, 0, %0, c9, c12, 1" : : "r" (val));
}

static inline void pmu_disable_counter(u32 val)
{
	val = 1 << val;
	asm volatile("mcr p15, 0, %0, c9, c12, 2" : : "r" (val));
}

static inline u32 pmu_select_counter(u32 idx)
{
	asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r" (idx));
	return idx;
}

static inline u32 pmnc_read(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));
	return val;
}

static inline void pmnc_write(u32 val)
{
	val &= 0x3f;
	asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(val));
}

static inline void pmnc_config(u32 counter, u32 event)
{
	counter = counter & 0x1F;
	asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r"(counter));
	asm volatile("mcr p15, 0, %0, c9, c13, 1" : : "r"(event));
}

static inline void pmnc_reset(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));
	val = val | 0x2;
	asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(val));
}

static inline u32 pmnc_read2(u32 counter)
{
	u32 ret;
	counter = counter & 0x1F;
	asm volatile("mcr p15, 0, %0, c9, c12, 5" : : "r"(counter));
	asm volatile("mrc p15, 0, %0, c9, c13, 2" : "=r"(ret));
	return ret;
}

static inline void enable_pmu(void)
{
	u32 val;

	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));
	val = val | 0x1;	// set E bit
	asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(val));
}

static inline void disable_pmu(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));
	val = val & ~0x1;	// clear E bit
	asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(val));
}

static inline u32 read_pmnc(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));
	return val;
}

static inline void disable_ccnt(void)
{
	u32 val = 0x80000000;
	asm volatile("mcr p15, 0, %0, c9, c12, 2" : : "r"(val));
}

static inline void enable_ccnt(void)
{
	u32 val = 0x80000000;
	asm volatile("mcr p15, 0, %0, c9, c12, 1" : : "r"(val));
}

static inline void reset_ccnt(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));
	val = val | 0x4;	// set C bit
	asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(val));
}

static inline u32 read_ccnt(void)
{
	u32 val;
	asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(val));
	return val;
}
#endif

struct pmu_count_value {
	u64	ccnt;
	u32	pmnc0;
	u32	pmnc1;
	u32	pmnc2;
	u32	pmnc3;
	u32	pmnc4;
	u32	pmnc5;
	u32	pmnc6;

	int	core_num;

	int	valid;
};

#define __start_timer(x) (enable_pmu(), disable_ccnt(), reset_ccnt(), enable_ccnt())
#define __start_timer_resume(x) (enable_pmu(), enable_ccnt())
#define __stop_timer(x) read_ccnt()

#if defined(CONFIG_PMU_COREMEM_RATIO)
int is_alive_cpu(int cpu);
int start_counter_cpu(int cpu);
int stop_counter_cpu(int cpu);
int read_pmu_one(void *data);
void read_counter_value(struct pmu_count_value *data);
int coremem_ratio (u32 instcnt, u32 linefcnt);
#else
static inline int is_alive_cpu(int cpu)
{
	return 1;
}

static inline int start_counter_cpu(int cpu)
{
	return 0;
}

static inline int stop_counter_cpu(int cpu)
{
	return 0;
}

static inline int read_pmu_one(void *data)
{
	return 0;
}

static inline void read_counter_value(struct pmu_count_value *data);
{
	return;
}

static inline int coremem_ratio (u32 instcnt, u32 linefcnt)
{
	return 0;
}
#endif

#define REGION_C100_M000_C080_M020	0x1
#define REGION_C080_M020_C060_M040	0x2
#define REGION_C060_M040_C040_M060	0x3
#define REGION_C040_M060_C020_M080	0x4
#define REGION_C020_M080_C000_M100	0x5

#endif	/* __PMU_FUNC_H_ */
