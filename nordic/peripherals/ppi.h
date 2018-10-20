/**
 * @file ppi.h Nordic's Programmable Peripheral Interface
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t ppi_channel_t;
typedef uint8_t ppi_group_t;

enum { ppi_channel_invalid = (ppi_channel_t)(-1) };

/**
 * Get the first free PPI channel available. The search starts from zero and
 * goes up. This relies on the PPI CH[].EEP and CH[].TEP to be set to zero.
 * If both are zero then the channel is available for use.
 * Only channels [0:19] are given out; Channels [20:31] are reserved for Nordic.
 *
 * @param task_register_pointer  The task register to bind with the PPI channel
 *                               if the channel is allocated successfully.
 * @param event_register_pointer The event register to bind the PPI channel with.
 * @param fork_register_pointer  The 2nd event register to bind with the PPI channel.
 *
 * @note Either or both of task_register_pointer, event_register_pointer must
 *       be non-null so that the PPI channel is reserved for use by the client.
 *       Since .EEP or .TEP are used to reserve the channel for the client.
 *
 * @return ppi_channel_t The index into the PPI channel list for the first unused
 * channel found, starting with index zero.
 * @retval ppi_channel_invalid if no PPI channels are avaiable.
 */
ppi_channel_t ppi_channel_allocate(uint32_t volatile *task_register_pointer,
                                   uint32_t volatile *event_register_pointer,
                                   uint32_t volatile *fork_register_pointer);

void ppi_channel_release(ppi_channel_t ppi_channel);

void ppi_channel_bind_task(ppi_channel_t ppi_channel, uint32_t volatile* task_register_pointer);
void ppi_channel_bind_event(ppi_channel_t ppi_channel, uint32_t volatile* event_register_pointer);
void ppi_channel_bind_fork(ppi_channel_t ppi_channel, uint32_t volatile* event_register_pointer);

void ppi_channel_enable(ppi_channel_t ppi_channel);
void ppi_channel_disable(ppi_channel_t ppi_channel);

/**
 * Find a free PPI channel group.
 * Not yet implemented.
 *
 * @return ppi_group_t The index into the PPI grouplist for the first unused
 * group found, starting with index zero.
 */
ppi_group_t ppi_channel_find_free_group(void);

#ifdef __cplusplus
}
#endif

