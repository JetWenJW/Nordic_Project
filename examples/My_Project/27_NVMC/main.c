#include <stdbool.h>
#include <stdio.h>
#include "nrf.h"
#include "bsp.h"
#include "app_error.h"
#include "nrf_nvmc.h"
#include "nordic_common.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "nrf_cli.h"
#include "nrf_cli_uart.h"


void log_init(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    log_init();
    NRF_LOG_INFO("Application Start...");

    /* 127 * 4 * 1024 (Last Page start Address) */
    uint32_t f_addr = 0x0007F000;
    uint32_t * p_addr = (uint32_t *)f_addr;
    uint32_t val = 666666;

    const uint32_t array[3] = {6, 66, 666};

    NRF_LOG_INFO("Erasing Page 127...");
    nrf_nvmc_page_erase(f_addr);
    NRF_LOG_INFO("Page Erase Successfully...");
    NRF_LOG_INFO(" Writing the Value to the given address...");

    nrf_nvmc_write_word(f_addr, val);
    NRF_LOG_INFO(" Value Writen Completed... ");
    NRF_LOG_INFO("The Data Read From Flash : %d ", *p_addr);

    nrf_nvmc_write_words(f_addr, array, 3);
    NRF_LOG_INFO(" Value Writen Array Completed... ");

    for(int i = 0;i < 3;++i)
    {
        NRF_LOG_INFO("The Data Read Array From Flash : %d ", *(p_addr + i));
    }

    while (true)
    {
        NRF_LOG_PROCESS();
    }
}
