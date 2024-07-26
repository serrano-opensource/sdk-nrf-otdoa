/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <modem/location.h>
#include <modem/lte_lc.h>
#include <nrf_modem_at.h>
#include <nrf_errno.h>
#include "location_core.h"
#include "location_utils.h"

LOG_MODULE_DECLARE(location, CONFIG_LOCATION_LOG_LEVEL);
static bool running;
static struct location_otdoa_config otdoa_config;
K_SEM_DEFINE(scan_otdoa_ready,0,1);
static struct k_work method_otdoa_start_work;

int method_otdoa_cancel(void)
{
        int err;

        LOG_WRN("in %s",__FUNCTION__);

        running = false;
//        k_sleep(K_MSEC(1000));
        /* Cancel any work that has not been started yet */
        (void)k_work_cancel(&method_otdoa_start_work);
        err = 0;
        return err;
}


static void method_otdoa_positioning_work_fn(struct k_work *work)
{
	static struct location_data location_result = { 0 };
	LOG_WRN("in %s",__FUNCTION__);
 
	time_t my_time;
	struct tm * timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);
 	location_result.latitude  = 33.129640;
	location_result.longitude = -117.32546;
	location_result.accuracy = 15.5;
	location_result.datetime.valid = true;
	location_result.datetime.year =  timeinfo->tm_year+1900;
	location_result.datetime.month = timeinfo->tm_mon+1;
	location_result.datetime.day =   timeinfo->tm_mday;
	location_result.datetime.hour =  timeinfo->tm_hour;
	location_result.datetime.minute = timeinfo->tm_min;
	location_result.datetime.second = timeinfo->tm_sec;
	location_result.datetime.ms = 100;	
	location_core_event_cb(&location_result);
	method_otdoa_cancel();
//	LOG_WRN("taking semaphore");
//	err = k_sem_take(&scan_otdoa_ready, K_SECONDS(5));
//	LOG_WRN("in %s err %d",__FUNCTION__,err);
//	LOG_WRN("giving semaphore");
//	k_sem_give(&scan_otdoa_ready);
// We probably want continuous OTDOA tracking mode.
}


void method_otdoa_event_handler(const struct lte_lc_evt *const evt)
{
	LOG_WRN("in %s evt->type %d",__FUNCTION__,evt->type);
	switch (evt->type) {
    case LTE_LC_EVT_NW_REG_STATUS:
		LOG_ERR("LTE_LC_NW_REG_STATUS");
		break;	
	case LTE_LC_EVT_PSM_UPDATE:
		LOG_ERR("LTE_LC_EVT_PSM_UPDATE");
		break;
	case LTE_LC_EVT_EDRX_UPDATE:
		LOG_ERR("LTE_LC_EVT_EDRX_UPDATE");
		break;	
	case LTE_LC_EVT_RRC_UPDATE:
		LOG_ERR("LTE_LC_EVT_RRC_UPDATE");
			break;	
	case LTE_LC_EVT_CELL_UPDATE:
		LOG_ERR("LTE_LC_EVT_CELL_UPDATE");
		break;		
	case LTE_LC_EVT_LTE_MODE_UPDATE:
		LOG_ERR("LTE_LC_EVT_MODE_UPDATE");
		break;	
	case LTE_LC_EVT_TAU_PRE_WARNING:
		LOG_ERR("LTE_LC_TAU_PRE_WARNING");
		break;		
	case LTE_LC_EVT_NEIGHBOR_CELL_MEAS:
		LOG_ERR("LTE_LC_EVT_NEIGHBOR_CELL_MEAS");
		break;	
	case LTE_LC_EVT_MODEM_SLEEP_EXIT_PRE_WARNING:
		LOG_ERR("LTE_LC_EVT_MODEM_SLEEP_EXIT_PRE_WARNING");
		break;			
	case LTE_LC_EVT_MODEM_SLEEP_EXIT:
		LOG_ERR("LTE_LC_EVT_MODEM_SLEEP_EXIT");
		break;	
	case LTE_LC_EVT_MODEM_SLEEP_ENTER:
		LOG_ERR("LTE_LC_EVT_MODEM_SLEEP_ENTER");
		break;	
	case LTE_LC_EVT_MODEM_EVENT:
		LOG_ERR("LTE_LC_EVT_MODEM_EVENT");
		break;														
	}
}



int method_otdoa_timeout(void)
{
	LOG_WRN("in %s",__FUNCTION__);
	return method_otdoa_cancel();
}

int method_otdoa_location_get(const struct location_request_info *request)
{
	int err;
	LOG_WRN("in %s",__FUNCTION__);
	otdoa_config = *request->otdoa;
	k_work_submit_to_queue(location_core_work_queue_get(), &method_otdoa_start_work);
	running = true;
    err = 0;
	return err;
}

int method_otdoa_init(void)
{
	int err;
	running = false;
	LOG_WRN("in %s",__FUNCTION__);  
	k_work_init(&method_otdoa_start_work, method_otdoa_positioning_work_fn);
//	k_sleep(K_MSEC(5000));
	//lte_lc_register_handler(method_otdoa_event_handler);
    err = 0;
	return err;
}
