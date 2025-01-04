
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

/*  Step 4th : Initial Power Management*/
static void power_management_init(void) 
{
  ret_code_t err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}

/* Step 4_1 : idle state handle */
static void idle_state_handle(void)
{
  if(NRF_LOG_PROCESS() == false)
  {
    nrf_pwr_mgmt_run();
  }
}


/* Step 3rd : Initial BSP(LED) */
static void leds_init(void)
{
  ret_code_t err_code = bsp_init(BSP_INIT_LEDS, NULL);
  APP_ERROR_CHECK(err_code);
}

/* Step 2nd : Initial App Timer */
static void timers_init(void)
{
  ret_code_t err_code = app_timer_init();
  APP_ERROR_CHECK(err_code);
}

/* Step 1st : Inital Log */
static void log_init()
{
  ret_code_t err_code = NRF_LOG_INIT(NULL) ;
  APP_ERROR_CHECK(err_code);
  
  NRF_LOG_DEFAULT_BACKENDS_INIT();
}





/**@brief Function for application main entry.
 */
int main(void)
{

    NRF_LOG_INFO("Project Start Jet");
    // Enter main loop.
    for (;;)
    {
      idle_state_handle();
    }
}


/**
 * @}
 */
