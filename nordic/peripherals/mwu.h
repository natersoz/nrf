/**
 * @file nordic/peripherals/mwu.h
 * Enable/Disable the nRF5x Memory watch unit.
 */

#ifdef __cplusplus
extern "C" {
#endif

void mwu_enable(void);

void mwu_disable(void);

void mwu_debug(void);

#ifdef __cplusplus
}
#endif
