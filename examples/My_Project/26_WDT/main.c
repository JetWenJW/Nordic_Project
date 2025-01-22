#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "bsp.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_drv_wdt.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "app_util_platform.h"


/* Step1. config WDT Channel */
nrfx_wdt_channel_id m_channel_id;

/* Step2_1 Create WDT Handler */
void wdt_event_handler(void)
{



}

/* Step2. Watch Dog Timer Initial */
void wdt_init(void)
{
    uint32_t err_code = NRF_SUCCESS;    
    nrfx_wdt_config_t wdt_config = NRFX_WDT_DEAFULT_CONFIG;

    /* Initial the WDT & bonding Handler */
    err_code = nrfx_wdt_init(&wdt_config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);

    /* alloc the channel handle */
    err_code = nrfx_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);

    /* Enable the WDT */
    nrfx_wdt_enable();
}





/*
 * Press the Button 2 longer enough
 * More than 2 seconds the WDT won't be feed
 * The Device will reset auto.
 */
int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);

    nrf_gpio_cfg_input(BUTTON_2, NRF_GPIO_PIN_PULLUP);
    
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

    for(int i = 0;i < 10;++i)
    {
        bsp_board_led_invert(0);
        nrf_delay_ms(100);
    }

    bsp_board_leds_off();
    wdt_init();

    while (1)
    {
        if(nrf_gpio_pin_read(BUTTON_2) == 0)
        {
            bsp_board_leds_on();
            while(nrf_gpio_pin_read(BUTTON_2) == 0);
            bsp_board_leds_off();
        }
        nrfx_wdt_channel_feed(m_channel_id);
    }
}

/** @} */
