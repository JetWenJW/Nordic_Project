#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define LED     17
#define LED_1   18
#define LED_2   19
#define LED_3   20

#define BTN     13
#define BTN_1   14
#define BTN_2   15
#define BTN_3   16
/**
 * @brief Function for application main entry.
 */
int main(void)
{
  nrf_gpio_cfg_input(BTN, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_output(LED);

  nrf_gpio_cfg_input(BTN_1, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_output(LED_1);

  nrf_gpio_cfg_input(BTN_2, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_output(LED_2);

  nrf_gpio_cfg_input(BTN_3, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_output(LED_3);

  nrf_gpio_pin_set(LED);      /* Clear the LED */
  nrf_gpio_pin_set(LED_2);    /* Clear the LED */
  nrf_gpio_pin_set(LED_1);    /* Clear the LED */
  nrf_gpio_pin_set(LED_3);    /* Clear the LED */
  
  while(1)
  {
      if(nrf_gpio_pin_read(BTN) == 0)
      {
          nrf_gpio_pin_clear(LED);
          while(nrf_gpio_pin_read(BTN) == 0);   /* Stay in the loop until release the BTN */
          nrf_gpio_pin_set(LED);
      }
      else if(nrf_gpio_pin_read(BTN_1) == 0 && nrf_gpio_pin_read(BTN_2) == 0)
      {
          while(nrf_gpio_pin_read(BTN_1) == 0 && nrf_gpio_pin_read(BTN_2) == 0);
          nrf_gpio_pin_toggle(LED_1);
          nrf_gpio_pin_toggle(LED_2);
      }


  }
}

/**
 *@}
 **/
