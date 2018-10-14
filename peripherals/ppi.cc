/**
 * @file ppi.h Nordic's Programmable Peripheral Interface
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ppi.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <iterator>

uint8_t ppi_channel_allocate(uint32_t volatile *task_register_pointer,
                             uint32_t volatile *event_register_pointer,
                             uint32_t volatile *fork_register_pointer)
{
    // task_register_pointer or event_register_pointer must be non-null.
    // Otherwise the channel is not reserved and may be given to the next
    // caller.
    ASSERT((task_register_pointer != nullptr) || (event_register_pointer != nullptr));
    NRF_PPI_Type* const ppi_registers = reinterpret_cast<NRF_PPI_Type *>(NRF_PPI_BASE);

    for (PPI_CH_Type const &ppi_ch : ppi_registers->CH)
    {
        if ((ppi_ch.EEP == 0u) && (ppi_ch.TEP == 0u))
        {
            uint8_t const ppi_channel = &ppi_ch - &ppi_registers->CH[0];

            ppi_channel_bind_task( ppi_channel, task_register_pointer);
            ppi_channel_bind_event(ppi_channel, event_register_pointer);
            ppi_channel_bind_fork( ppi_channel, fork_register_pointer);

            return ppi_channel;
        }
    }

    // None are avaiable. This is not expected to happen.
    ASSERT(0);
    return ppi_channel_invalid;
}

void ppi_channel_release(uint8_t ppi_channel)
{
    // Allow the release function to be called for non-allocated PPI channels.
    // Silently ignore it.
    if (ppi_channel != ppi_channel_invalid)
    {
        NRF_PPI_Type* const ppi_registers = reinterpret_cast<NRF_PPI_Type *>(NRF_PPI_BASE);
        ASSERT(ppi_channel < std::size(ppi_registers->CH));
        ppi_channel_disable(ppi_channel);

        ppi_channel_bind_task( ppi_channel, nullptr);
        ppi_channel_bind_event(ppi_channel, nullptr);
        ppi_channel_bind_fork( ppi_channel, nullptr);
    }
}

void ppi_channel_bind_task(uint8_t ppi_channel, uint32_t volatile *task_register_pointer)
{
    NRF_PPI_Type* const ppi_registers = reinterpret_cast<NRF_PPI_Type *>(NRF_PPI_BASE);
    ASSERT(ppi_channel < std::size(ppi_registers->CH));

    ppi_registers->CH[ppi_channel].TEP = reinterpret_cast<uint32_t>(task_register_pointer);
}

void ppi_channel_bind_event(uint8_t ppi_channel, uint32_t volatile *event_register_pointer)
{
    NRF_PPI_Type* const ppi_registers = reinterpret_cast<NRF_PPI_Type *>(NRF_PPI_BASE);
    ASSERT(ppi_channel < std::size(ppi_registers->CH));

    ppi_registers->CH[ppi_channel].EEP = reinterpret_cast<uint32_t>(event_register_pointer);
}

void ppi_channel_bind_fork(uint8_t ppi_channel, uint32_t volatile *event_register_pointer)
{
    NRF_PPI_Type* const ppi_registers = reinterpret_cast<NRF_PPI_Type *>(NRF_PPI_BASE);

    // Note: even though FORK[] is allocated larger than CH[],
    // use size of CH[] as the ASSERT check.
    // The upper 12 channels are Nordic reserved.
    ASSERT(ppi_channel < std::size(ppi_registers->CH));

    ppi_registers->FORK[ppi_channel].TEP = reinterpret_cast<uint32_t>(event_register_pointer);
}

void ppi_channel_enable(uint8_t ppi_channel)
{
    NRF_PPI_Type* const ppi_registers = reinterpret_cast<NRF_PPI_Type *>(NRF_PPI_BASE);
    ASSERT(ppi_channel < std::size(ppi_registers->CH));

    ppi_registers->CHENSET = (1u << ppi_channel);
}

void ppi_channel_disable(uint8_t ppi_channel)
{
    NRF_PPI_Type* const ppi_registers = reinterpret_cast<NRF_PPI_Type *>(NRF_PPI_BASE);
    ASSERT(ppi_channel < std::size(ppi_registers->CH));

    ppi_registers->CHENCLR = (1u << ppi_channel);
}

