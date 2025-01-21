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


void sddac_callback_handler(nrf_drv_saadc_evt_t const * p_evt)
{
    /* Empty */
}


void log_init(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


void saadc_init(void)
{
    ret_code_t err_code;
    
    /*   
     * Create a config struct and assignit default value
     * As a Single end Pin
     */
    nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2); 
    
    err_code = nrf_drv_saadc_init(NULL, sddac_callback_handler);    
    APP_ERROR_CHECK(err_code);

    /* Initial the channel which will be connect to the specific Pin */
    err_code = nrfx_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);
}





/**
 * @brief Function for main application entry.
 */
int main(void)
{

    log_init();
    saadc_init();

    nrf_saadc_value_t adc_value;
    NRF_LOG_INFO("Application SAADC Start...");

    while (1)
    {
        NRF_LOG_PROCESS();
        nrfx_saadc_sample_convert(0, &adc_value);
        NRF_LOG_INFO("Voltage Read : %d", adc_value);
        NRF_LOG_INFO("Volts : " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(adc_value * 3.6 / 1024));
        nrf_delay_ms(500);
    }
}


/** @} */
