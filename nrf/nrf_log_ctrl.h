/**
 * @file nrf_log_ctrl.h
 */

#pragma once
#include "logger_c.h"

#define NRF_LOG_PROCESS()       logger_process()

#define NRF_LOG_FLUSH()         logger_flush()
#define NRF_LOG_FINAL_FLUSH()   logger_flush()

