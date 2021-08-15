#include "nrf_log.h"

/** @brief Check if the error code is equal to NRF_SUCCESS. If it is not, return the error code.
 */
#define LOG_ERROR(text, statement)                                                                                                                             \
    do {                                                                                                                                                       \
        uint32_t _err_code = (uint32_t)(statement);                                                                                                            \
        if (_err_code != NRF_SUCCESS) {                                                                                                                        \
            NRF_LOG_ERROR("%s error, code: %u, %s!", text, _err_code, NRF_LOG_ERROR_STRING_GET(_err_code));                                                    \
            NRF_LOG_ERROR("In file: %s line: %d", __FILE__, __LINE__);                                                                                         \
        }                                                                                                                                                      \
    } while (0)

#define ASSERT_TRUE(expression)                                                                                                                                \
    do {                                                                                                                                                       \
        if (!(bool)(expression)) {                                                                                                                             \
            NRF_LOG_ERROR("Assert failed!");                                                                                                                   \
            NRF_LOG_ERROR("In file: %s line: %d", __FILE__, __LINE__);                                                                                         \
            return;                                                                                                                                            \
        }                                                                                                                                                      \
    } while (0)
