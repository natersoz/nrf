/**
 * @file ppi.h Nordic's Programmable Peripheral Interface
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { ppi_channel_invalid = UINT8_MAX };

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
 * @return uint8_t The index into the PPI channel list for the first unused
 * channel found, starting with index zero.
 * @retval ppi_channel_invalid if no PPI channels are avaiable.
 */
uint8_t ppi_channel_allocate(uint32_t volatile *task_register_pointer,
                             uint32_t volatile *event_register_pointer,
                             uint32_t volatile *fork_register_pointer);

void ppi_channel_release(uint8_t ppi_channel);

void ppi_channel_bind_task(uint8_t ppi_channel, uint32_t volatile* task_register_pointer);
void ppi_channel_bind_event(uint8_t ppi_channel, uint32_t volatile* event_register_pointer);
void ppi_channel_bind_fork(uint8_t ppi_channel, uint32_t volatile* event_register_pointer);

void ppi_channel_enable(uint8_t ppi_channel);
void ppi_channel_disable(uint8_t ppi_channel);

/**
 * Find a free PPI channel group.
 * Not yet implemented.
 *
 * @return uint8_t The index into the PPI grouplist for the first unused
 * group found, starting with index zero.
 */
uint8_t ppi_channel_find_free_group(void);



#ifdef __cplusplus
}
#endif

