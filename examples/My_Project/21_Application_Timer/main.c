#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"
#include "app_timer.h"
#include "nrf_clock.h"


#define LED 17
#define LED_INTERVAL APP_TIMER_TICKS(100)
APP_TIMER_DEF(m_app_timer_id);

void lfclk
/**
 * @brief Function for application main entry.
 */
int main(void)
{
    while (true)
    {
        // Do nothing.
    }
}
/** @} */
