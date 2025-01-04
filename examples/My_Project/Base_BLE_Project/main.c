
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

#include  "nrf_ble_qwr.h"

#define APP_BLE_CONN_CFG_TAG      1
#define APP_BLE_OBSERVER_PRIO     3

#define DEVICE_NAME               "Jet_BLE"

/* Setting GAP Transmit INTERVAL */
#define MIN_CONN_INTERVAL         MSEC_TO_UINTS(100, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL         MSEC_TO_UINTS(200, UNIT_1_25_MS)
#define SLAVE_LATENCY             0
#define CONN_SUP_TIMEOUT          MSEC_TO_UINT(2000, UINT_10_MS)



NRF_BLE_QWR_DEF(m_qwr);

static uint32_t m_conn_handle = BLE_CONN_HANDLE_INVALID;


/* Step 6 : Initial GAP */
static void gap_params_init(void)
{
  ret_code_t err_code;
  ble_gap_conn_params_t     gap_conn_params = {0};
  ble_gap_conn_sec_mode_t   sec_mode;
  
  /* Setting security mode(there are more options if you need) */
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  /* Set Device Name */
  err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
  APP_ERROR_CHECK(err_code);

  /* Set the value of gap_conn_parames Structure(Basic Setting is under below) */
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);     /* 把剛剛參數設定進去GAP */
  APP_ERROR_CHECK(err_code);
}




/* Step 5_1 : BLE Event Handler */
static void ble_evt_handler(ble_evt_t * const p_ble_evt, void * p_context)
{
  ret_code_t err_code = NRF_SUCCESS;
  switch(p_ble_evt->header.evt_id)
  {
  
    case BLE_GAP_EVT_DISCONNECTED :
        NRF_LOG_INFO(" The Device is Disconnected Jet~~");



    break;
  
    case BLE_GAP_EVT_CONNECTED : 
        NRF_LOG_INFO(" The Device is Connected Jet~~");
        err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);                    /* Set the Connection Mode           */
        APP_ERROR_CHECK(err_code);
        
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;                       /* Asigned the value to Queue        */
        err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);         /* Assign the m_conn_handle to m_qwr */
        APP_ERROR_CHECK(err_code);
    break;
  
    case BLE_GAP_EVT_PHY_UPDATE_REQUEST : /* Change transmit speed */
        NRF_LOG_DEBUG("PHY Wanna Update Speed Jet~");
        ble_gap_phys_t const phys = 
        {
            .tx_phys = BLE_GAP_PHY_AUTO,
            .rx_phys = BLE_GAP_PHY_AUTO,
        };  /* They'll assign the value automatically to update PHY Speed */

        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    break;
  }
}

/* Step 5 : Initial BLE Stack  */
static void ble_stack_init(void)
{
  ret_code_t err_code;
  err_code = nrf_sdh_enable_request();      /* Sent request to enable SoftDevice(BLE_Stack) */ 
  APP_ERROR_CHECK(err_code);

  uint32_t ram_start = 0;
  err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);   /* Assigned a RAM to store SoftDevice */
  APP_ERROR_CHECK(err_code);

  err_code = nrf_sdh_ble_enable(&ram_start);    /* Enable SoftDevice(BLE_Stack) */
  APP_ERROR_CHECK(err_code);

  NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);   /* Assignec aBLE Event Handler */
}



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
