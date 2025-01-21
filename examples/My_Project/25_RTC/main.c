#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "boards.h"
#include "app_error.h"
#include <stdint.h>
#include <stdbool.h>

#define LED3  19

const nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(2);    /* RTC2 because RTC1 &RTC0 are common use in the Softdevice */

static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

}


static void rtc_handler(nrfx_rtc_int_type_t int_type)
{
    if(int_type == NRFX_RTC_INT_TICK)
    {
        nrf_gpio_pin_toggle(LED3);
    }
}

static void rtc_config(void)
{
    uint32_t err_code;
    nrfx_rtc_config_t rtc_config = NRFX_RTC_DEFAULT_CONFIG;
    
    /* tick = 32768 / (4095 + 1) = 8Hz = 125ms */
    rtc_config.prescaler = 4095;

    err_code = nrfx_rtc_init(&rtc, &rtc_config, rtc_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_rtc_tick_enable(&rtc, true);
    nrfx_rtc_enable(&rtc);
}


/* In this project we use RTC2 to Demo. */
int main(void)
{
    
    bsp_board_init(BSP_INIT_LEDS);

    lfclk_config();

    rtc_config();
    
    while (true)
    {
        __SEV();
        __WFE();
        __WFE();
    }
}


/**  @} */
