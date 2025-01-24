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


#define m_top         5000
#define m_step        100
#define led_external  22


static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);
static nrf_pwm_values_grouped_t sequence_value[m_step * 2];
uint16_t step = m_top / m_step;
uint16_t value = 0;

static void pwm_handler(nrfx_pwm_evt_type_t event_type)
{
    if(event_type == NRFX_PWM_EVT_FINISHED)
    {
        NRF_LOG_INFO("Finished Play in Sequence Once...");
    }
}


static void pwm_common_init(void)
{
    for(int i = 0;i < m_step;++i)
    {
        value += step;    
        sequence_value[i].group_0 = value;
        sequence_value[i].group_1 = value;

        sequence_value[m_step + i].group_0 = m_top - value;
        sequence_value[m_step + i].group_1 = m_top - value;
    
    }
    
    nrfx_pwm_config_t const config0 = 
    {
        .output_pins=
        {
            BSP_LED_0 | NRFX_PWM_PIN_INVERTED,
            BSP_LED_1 | NRFX_PWM_PIN_INVERTED,
            BSP_LED_2 | NRFX_PWM_PIN_INVERTED,
            BSP_LED_3 | NRFX_PWM_PIN_INVERTED,
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = m_top,
        .load_mode    = NRF_PWM_LOAD_GROUPED,
        .step_mode    = NRF_PWM_STEP_AUTO 
    };
    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config0, pwm_handler));
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code); 

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


static void pwm_play(void)
{
    nrf_pwm_sequence_t const seq0 =
    {
        .values.p_grouped  = sequence_value,
        .length            = NRF_PWM_VALUES_LENGTH(sequence_value),
        .repeats           = 0,
        .end_delay         = 0
    };
    (void)nrfx_pwm_simple_playback(&m_pwm0, &seq0, 1, NRFX_PWM_FLAG_LOOP);

}


int main(void)
{
    log_init();
    pwm_common_init();
    bsp_board_init(BSP_INIT_LEDS);
    pwm_play();
    NRF_LOG_INFO("PWM Application Start...");

    while(true)
    {
        NRF_LOG_PROCESS();
    }
    

}


/** @} */
