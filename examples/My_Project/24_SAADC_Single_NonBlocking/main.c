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
    NRF_LOG_INFO("SAADC Application Start...");

    while (1)
    {
        NRF_LOG_PROCESS();
        nrfx_saadc_sample();
        nrf_delay_ms(1000);
    }
}


/** @} */
