
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "nrf_log.h"

#define LED1 17
#define LED2 18
#define LED3 19
#define LED4 20

#define BTN1 13
#define BTN2 14
#define BTN3 15
#define BTN4 16

static nrf_drv_timer_t timer = NRF_DRV_TIMER_INSTANCE(0);

void timer0_interrupt_handler(nrf_timer_event_handler_t evt, void * p_context)
{
    NRF_LOG_INFO("Jet");
}
static void start_timer_led(void)
{
    uint32_t compare_event_addr;
    uint32_t gpiote_task_addr;
    nrf_ppi_channel_t ppi_channel;
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
    
    err_code = nrf_drv_gpiote_out_init(LED2, &out_config);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_config = NRF_DRV_TIMER_DEFAULT_CONFIG;

    err_code = nrf_drv_timer_init(&timer, &timer_config, timer0_interrupt_handler);
    APP_ERROR_CHECK(err_code);

    uint32_t time_ticks = nrfx_timer_ms_to_ticks(&timer, 2000);
    nrf_drv_timer_extended_compare(&timer, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    compare_event_addr = nrf_drv_timer_event_address_get(&timer, NRF_TIMER_EVENT_COMPARE0);
    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(LED2);

    err_code = nrf_drv_ppi_channel_assign(ppi_channel, compare_event_addr, gpiote_task_addr);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(err_code);
    
    /* In is Event; Out is Task */
    nrf_drv_gpiote_out_task_enable(LED2);
}


int main(void)
{
    start_timer_led();
    nrfx_timer_enable(&timer);

    while (true)
    {
        // Do Nothing - GPIO can be toggled without software intervention.
    }
}


/** @} */
