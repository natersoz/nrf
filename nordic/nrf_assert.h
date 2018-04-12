/**
 * @file nrf_assert.h
 * Substitute out the Nordic ASSERT() macro
 */

#pragma once

// This is to satisfy many of the nordic files which include "nrf_assert.h"
// to obtain "nrf.h", "app_error.h"
#include <stdint.h>
#include "nrf.h"
#include "app_error.h"


#include "project_assert.h"

