#include <stdio.h>
#include <string.h>
#include "nrf_drv_pwm.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"

#define m_top         100

#define LED_1   17
#define LED_2   18
#define LED_3   19

static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

nrf_pwm_values_individual_t seq_values[] = {0, 0, 0, 0};


nrf_pwm_sequence_t const seq0 = 
{
    .values.p_individual  = seq_values,
    .length               = NRF_PWM_VALUES_LENGTH(seq_values),
    .repeats              = 0,
    .end_delay            = 0
};

static void pwm_update_duty_cycle(uint8_t duty_LED_1,
                                  uint8_t duty_LED_2,
                                  uint8_t duty_LED_3
                                 )
{
    if(duty_LED_1 >= 100)
    {
        seq_values->channel_0 = 100;
    }
    else
    {
        seq_values->channel_0 = duty_LED_1;
    }

    if(duty_LED_2 >= 100)
    {
        seq_values->channel_1 = 100;
    }
    else
    {
        seq_values->channel_1 = duty_LED_2;
    }

    if(duty_LED_3 >= 100)
    {
        seq_values->channel_2 = 100;
    }
    else
    {
        seq_values->channel_2 = duty_LED_3;
    }

    (void)nrfx_pwm_simple_playback(&m_pwm0, &seq0, 1, NRFX_PWM_FLAG_LOOP);
}






static void pwm_init(void)
{
    
    nrfx_pwm_config_t const config0 = 
    {
        .output_pins=
        {
            LED_1,
            LED_2,
            LED_3,
            NRFX_PWM_PIN_NOT_USED  
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = m_top,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO 
    };
    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config0, NULL));
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code); 

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


int main(void)
{
    log_init();
    
    bsp_board_init(BSP_INIT_LEDS);

    pwm_init();
 
    NRF_LOG_INFO("PWM Application Start...");

    while(true)
    {
        NRF_LOG_PROCESS();
        pwm_update_duty_cycle(100, 100, 100);
        nrf_delay_ms(1000);
        
        pwm_update_duty_cycle(0, 0, 0);
        nrf_delay_ms(1000);
    }
}


/** @} */
