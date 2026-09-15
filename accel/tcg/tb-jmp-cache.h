/*
 * The per-CPU TranslationBlock jump cache.
 *
 *  Copyright (c) 2003 Fabrice Bellard
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "qemu/rcu.h"
#include "exec/cpu-common.h"

#define TB_JMP_CACHE_BITS 12
#define TB_JMP_CACHE_SIZE (1 << TB_JMP_CACHE_BITS)

/*
 * Two entries per set. A jump lands in the cache by the address it jumps to,
 * and a direct-mapped cache this size throws away a quarter of what it is
 * asked for: on this guest, a quarter of every indirect jump went to the hash
 * table instead. A second way halves the conflicts without costing a byte more
 * memory, without making a flush any dearer, and without a second cache line --
 * the two entries of a set are neighbours.
 *
 * Making the cache larger instead was tried: four times the entries did cut the
 * misses, but the array no longer fits the host's caches and a flush -- which
 * this guest does thousands of times a second, on every change of address space
 * -- has four times as much to clear.
 */
#define TB_JMP_CACHE_WAYS 2

/*
 * Invalidated in parallel; all accesses to 'tb' must be atomic.
 * A valid entry is read/written by a single CPU, therefore there is
 * no need for qatomic_rcu_read() and pc is always consistent with a
 * non-NULL value of 'tb'.  Strictly speaking pc is only needed for
 * CF_PCREL, but it's used always for simplicity.
 */
typedef struct CPUJumpCache
{
    struct rcu_head rcu;
    struct
    {
        TranslationBlock* tb;
        vaddr             pc;
    } array[TB_JMP_CACHE_SIZE];
} CPUJumpCache;
