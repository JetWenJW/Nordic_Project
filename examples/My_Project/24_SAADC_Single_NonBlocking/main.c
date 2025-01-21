#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/*
 *   8 bit  = 12.89 milli volts
 *   10 bit = 3.226 milli volts
 *   12 bit = 805.66 Micro volts
 *   14 bit = 204.41 Micro Volts
 */

#define SAMPLE_BUFFER_LEN 5
static nrf_saadc_value_t m_buffer_pool[2][SAMPLE_BUFFER_LEN];

/* Declait Timer & PPI Channel */
static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(1);
static nrf_ppi_channel_t m_ppi_channel;

void timer_handler(nrf_timer_event_t event_type, void * p_context)
{


}

void timer_with_ppi_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;

    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, 50);

    nrf_drv_timer_extended_compare(&m_timer, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_event_compare_addr = nrf_drv_timer_compare_event_address_get(&m_timer, NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_task_addr = nrf_drv_saadc_sample_task_get();

    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel, timer_event_compare_addr, saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}




/* SAADC Call Back Handler Function */
void saadc_callback_handler(nrf_drv_saadc_evt_t const * p_evt)
{
    float value;
    if(p_evt->type == NRFX_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
        err_code = nrfx_saadc_buffer_convert(p_evt->data.done.p_buffer, SAMPLE_BUFFER_LEN);
        APP_ERROR_CHECK(err_code);
        
        
        NRF_LOG_INFO("ADC Event Occured...");
        for(int i = 0;i < SAMPLE_BUFFER_LEN;++i)
        {
            NRF_LOG_INFO("Sample Value Read : %d", p_evt->data.done.p_buffer[i]);
            value = p_evt->data.done.p_buffer[i] * 3.6 / 4096;
            NRF_LOG_INFO("Voltage Read : " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(value));
        }
    }
}




void saadc_init(void)
{

    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN5);

    err_code = nrf_drv_saadc_init(NULL, saadc_callback_handler);
    APP_ERROR_CHECK(err_code);
 
    err_code = nrfx_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);
 
    err_code = nrfx_saadc_buffer_convert(m_buffer_pool[0], SAMPLE_BUFFER_LEN);
    APP_ERROR_CHECK(err_code);
    
    err_code = nrfx_saadc_buffer_convert(m_buffer_pool[1], SAMPLE_BUFFER_LEN);
    APP_ERROR_CHECK(err_code);

}



int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    saadc_init();

    timer_with_ppi_init();

    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("SAADC Application Start...");

    while (1)
    {
        NRF_LOG_PROCESS();

    }
}


/** @} */
