/**
 * @file nordic/app_error_fault_handler.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include <cstdint>
#include "nrf_sdm.h"
#include "app_error.h"
#include "logger.h"

extern "C"
void app_error_fault_handler(uint32_t error_code, uint32_t pc, uint32_t info)
{
    logger& logger = logger::instance();
    logger.flush();

    switch (error_code)
    {
    case NRF_FAULT_ID_SD_ASSERT:
        logger.error("SOFTDEVICE: ASSERTION FAILED, pc: %u, info: %u", pc, info);
        break;

    case NRF_FAULT_ID_APP_MEMACC:
        logger.error("SOFTDEVICE: INVALID MEMORY ACCESS, pc: %u, info: %u", pc, info);
        break;

    case NRF_FAULT_ID_SDK_ASSERT:
        {
            assert_info_t const* p_info = reinterpret_cast<assert_info_t const*>(info);
            logger.error("ASSERTION FAILED at %s:%u", p_info->p_file_name,
                                                      p_info->line_num);
        }
        break;

        case NRF_FAULT_ID_SDK_ERROR:
        {
             error_info_t const* p_info = reinterpret_cast<error_info_t const*>(info);
             logger.error("SDK ERROR 0x%04x at %s:%u", p_info->err_code,
                                                       p_info->p_file_name,
                                                       p_info->line_num);
        }
        break;

        default:
            logger.error("UNKNOWN FAULT at %u", pc);
            break;
    }

    __asm("bkpt #0");   // Debugger breakpoint
}
