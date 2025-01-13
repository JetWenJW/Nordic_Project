
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_uart.h"


#define UART_TX_BUFFER_SIZE 128
#define UART_RX_BUFFER_SIZE 128

#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED


void uart_error_handler(app_uart_evt_type_t * p)
{

}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
  uint32_t err_code;

  bsp_board_init(BSP_INIT_LEDS);

  const app_uart_comm_params_t com_params =
  {
    RX_PIN_NUMBER,
    TX_PIN_NUMBER,
    RTS_PIN_NUMBER,
    CTS_PIN_NUMBER,
    UART_HWFC,
    false,
    NRF_UART_BAUDRATE_115200
  };

  APP_UART_FIFO_INIT(&com_params,
                    UART_RX_BUFFER_SIZE,
                    UART_TX_BUFFER_SIZE,
                    uart_error_handler,
                    APP_IRQ_PRIORITY_LOWEST,
                    err_code
                    );
  APP_ERROR_CHECK(err_code);

  printf("\r\n Hello world Jet~~ \r\n");
 
  while(1)
  {
    uint8_t ch;

    while(app_uart_get(&ch) != NRF_SUCCESS);
    if(ch == 't')
    {
      bsp_board_leds_on();
      printf("LED Turned On\r\n");
    }
    if(ch == 'k')
    {
      bsp_board_leds_off();
      printf("LED Turned off\r\n");
    }
  }
}


/** @} */
