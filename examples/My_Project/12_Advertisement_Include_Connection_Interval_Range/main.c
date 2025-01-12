
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

#include "nrf_ble_qwr.h"
#include "nrf_ble_gatt.h"

#include "ble_advdata.h"
#include "ble_advertising.h"

#include "ble_conn_params.h"

#define APP_BLE_CONN_CFG_TAG      1
#define APP_BLE_OBSERVER_PRIO     3

#define DEVICE_NAME               "Jet_BLE"

/* Setting GAP Transmit INTERVAL */
#define MIN_CONN_INTERVAL         MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL         MSEC_TO_UNITS(200, UNIT_1_25_MS)
#define SLAVE_LATENCY             0
#define CONN_SUP_TIMEOUT          MSEC_TO_UNITS(2000, UNIT_10_MS)


#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(3000)
#define MAX_CONN_PARAMS_UPDATE_COUNT     3

/* Definition for Advertisement */
#define APP_ADV_INTERVAL          300
#define APP_ADV_DURATION          0


NRF_BLE_QWR_DEF(m_qwr);
NRF_BLE_GATT_DEF(m_gatt);
BLE_ADVERTISING_DEF(m_advertising);


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

/* Step7 : GATT Initial */
static void gatt_init(void)
{
  ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
  APP_ERROR_CHECK(err_code);
}

/* Step9_1 : Error Handler for Queue Writer */
static void nrf_qwr_err_handler(uint32_t nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}


/* Step9 : Initial The Service */
static void service_init(void)
{
  ret_code_t err_code;
  nrf_ble_qwr_init_t  qwr_init = {0};
  qwr_init.error_handler = nrf_qwr_err_handler;
  err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  APP_ERROR_CHECK(err_code);
}

/* Step 10_1 : Create an Event Handler for Connection Parameters Update */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
  ret_code_t err_code = 0;
  if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
  {
    err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);  
    APP_ERROR_CHECK(err_code);
  }
  if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_SUCCEEDED)
  {
    
  
    NRF_LOG_INFO("Connection Succeed HaHa");
  
  }
}

/* Step10_2 : Create an Error Handler for Connection Parameters Update */
static void conn_params_error_handler(uint32_t nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}



/* Step10 : Create Function for Setting Connection Parameters */
static void conn_params_init(void)
{
  ret_code_t err_code;
  ble_conn_params_init_t cp_init = {0};

  cp_init.p_conn_params                   = NULL;
  cp_init.first_conn_params_update_delay  = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay   = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count    = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle     = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail              = false;
  cp_init.error_handler                   = conn_params_error_handler;
  cp_init.evt_handler                     = on_conn_params_evt;

  err_code = ble_conn_params_init(&cp_init);
  APP_ERROR_CHECK(err_code);
}

/*Step8_1 : Crate Advertisement Event Handler */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
  ret_code_t err_code;
  switch(ble_adv_evt)
  {
    case BLE_ADV_EVT_FAST :
        NRF_LOG_INFO("Fast Advertising ...haha");
        err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
        APP_ERROR_CHECK(err_code);
    break;

    case BLE_ADV_EVT_IDLE:
        err_code = bsp_indication_set(BSP_INDICATE_IDLE);
        APP_ERROR_CHECK(err_code);
    break;

    default:
    break;
  }
}

/* Step 8 : Advertising Initial */
static void advertising_init(void)
{
  ret_code_t err_code;

  ble_advertising_init_t init = {0};

  /* Create a Structure Variable for Connection Interval Range */
  ble_advdata_conn_int_t conn_int_range;
  
  /* Settiing what contains in the advertising */
  init.advdata.name_type            = BLE_ADVDATA_FULL_NAME;
  init.advdata.include_appearance   = true;                                         
  init.advdata.flags                = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

  /* Set the Params for Connection Interval Range 
   * Just Add them into Advertisement Data,
   * even though they had configured in GAP & GATT 
   */
  conn_int_range.max_conn_interval  = MAX_CONN_INTERVAL;
  conn_int_range.min_conn_interval  = MIN_CONN_INTERVAL;

  init.advdata.p_slave_conn_int = &conn_int_range;

  init.config.ble_adv_fast_enabled  = true;
  init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
  init.evt_handler                  = on_adv_evt;
  
  err_code = ble_advertising_init(&m_advertising, &init);
  APP_ERROR_CHECK(err_code);

  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
          
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


/* Step11 : Create Function for starting advertisement */
static void advertising_start(void)
{
  ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
}

/**@brief Function for application main entry.
 */
int main(void)
{
    /* Put All the Initial Function into main function
     * Remember : the order shouldn't be disturb
     */
    log_init();
    timers_init();
    leds_init();
    power_management_init();

    ble_stack_init();
    gap_params_init();
    gatt_init();

    advertising_init();
    service_init();
    conn_params_init();
    NRF_LOG_INFO("BLE Base APP Start...HaHa");

    advertising_start();



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
