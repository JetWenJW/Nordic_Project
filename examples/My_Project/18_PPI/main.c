

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

#define LED 17
#define BTN 13

/* Declair a PPI Channel */
static nrf_ppi_channel_t ppi_channel;

void interrupt_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{


}

static void gpio_init()
{
  uint32_t err_code = NRF_SUCCESS;
  err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  in_config.pull = NRF_GPIO_PIN_PULLUP;

  err_code = nrf_drv_gpiote_in_init(BTN, &in_config, interrupt_pin_handler);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(true);
  err_code = nrf_drv_gpiote_out_init(LED, &out_config);
  APP_ERROR_CHECK(err_code); 

  nrf_drv_gpiote_out_task_enable(LED);
  nrf_drv_gpiote_in_event_enable(BTN, true);

}

static void ppi_init(void)
{
    uint32_t err_code      = NRF_SUCCESS;
    uint32_t BTN_evt_addr  = 0;
    uint32_t LED_task_addr = 0;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);
    APP_ERROR_CHECK(err_code);
    
    BTN_evt_addr = nrf_drv_gpiote_in_event_addr_get(BTN);
    LED_task_addr = nrf_drv_gpiote_out_task_addr_get(LED);

    err_code = nrf_drv_ppi_channel_assign(ppi_channel, BTN_evt_addr, LED_task_addr);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(err_code);
}




/**
 * @brief Function for application main entry.
 */
int main(void)
{
    gpio_init();
    ppi_init();

    while (true)
    {
        // Do Nothing - GPIO can be toggled without software intervention.
    }
}


/** @} */
