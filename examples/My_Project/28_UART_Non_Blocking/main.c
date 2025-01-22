#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"

#include "nrf_uarte.h"

#define UART_TX_BUF_SIZE 64
#define UART_RX_BUF_SIZE 64

#define LED_1 17
#define LED_2 18
#define LED_3 19
#define LED_4 20


/* Step1.1 uart handler */
void uart_handler(app_uart_evt_t * p_event)
{
    uint8_t c;
    if(p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    { 
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if(p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
    else if(p_event->evt_type == APP_UART_DATA_READY)
    {
        app_uart_get(&c);
        printf("the char Get from Uart is %c", c);
    }
    else if(p_event->evt_type == APP_UART_TX_EMPTY)
    {
        nrf_gpio_pin_toggle(LED_3);
        nrf_gpio_pin_toggle(LED_2);
    }

}



/* Step1. UART Config */
void uart_config(void)
{
    uint32_t err_code;
    const app_uart_comm_params_t comm_params = 
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        NRF_UARTE_BAUDRATE_115200
    };
    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_handler,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);

}

int main(void)
{
    nrf_gpio_cfg_output(LED_1);
    nrf_gpio_cfg_output(LED_2);
    nrf_gpio_cfg_output(LED_3);
    nrf_gpio_cfg_output(LED_4);
    uart_config();
    while (true)
    {
    
    
    }
}


/** @} */
