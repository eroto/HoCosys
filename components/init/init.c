#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
//#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>
#include "driver/temperature_sensor.h"
#include "leds.h"
#include "apptask_if.h"
#include "sense_if.h"
#include "I2C_if.h"
#include "blufi_if.h"
#include "consola_if.h"
#include "init.h"

//uint8_t s_led_state = 0;


static const char *TAG = "InitModule";

temperature_sensor_handle_t IntTempSensorHanlder = NULL;
shutdown_handler_t AppShutDownHandler = NULL;
void PreShutdown(void);


void PreShutdown(void)
{
	sense_deinit();
}

 char * ResetResons[11] = {
		    "UNKNOWN",    //!< Reset reason can not be determined
		    "POWERON",    //!< Reset due to power-on event
		    "EXT",        //!< Reset by external pin (not applicable for ESP32)
		    "SW",         //!< Software reset via esp_restart
		    "PANIC",      //!< Software reset due to exception/panic
		    "INT_WDT",    //!< Reset (software or hardware) due to interrupt watchdog
		    "RST_TASK_WDT",   //!< Reset due to task watchdog
		    "RST_WDT",        //!< Reset due to other watchdogs
		    "RST_DEEPSLEEP",  //!< Reset after exiting deep sleep mode
		    "RST_BROWNOUT",   //!< Brownout reset (software or hardware)
		    "RST_SDIO"		//!< Reset over SDIO
 };

uint8_t sysinit1(void)
{
	esp_err_t result =  ESP_FAIL;
	esp_reset_reason_t RstCode = esp_reset_reason();

	ESP_LOGI(TAG,"Reaset Reason code %s", ResetResons[RstCode]);

	/*Register the handler that gets invoked before the application
	 * is restarted using esp_restart function */
	AppShutDownHandler = PreShutdown;

	/*initialize Internal Temperature Sensor and analog inputs*/
	result = sense_init();

	/*Initialize Digital Outputs*/
	sense_init_DOs();
	ESP_LOGI(TAG, "Digital Outputs initialized");

	/*Initialize Digital Inputs*/
	sense_init_DIs();
	ESP_LOGI(TAG, "Digital Inputs initialized");

	/*Initialize I2C*/
	I2C_init();

	/*Initialize I2C Master*/
	ESP_ERROR_CHECK(i2c_slave_init());
	ESP_LOGI(TAG, "i2c slave initialized");

	/*result = i2c_slave_init();
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "i2c_slave_init Initialization Error");
		return result;
	}
	else{
		ESP_LOGI(TAG, "i2c slave initialized");
	}*/

	/*Initialize I2C Master*/
	ESP_ERROR_CHECK(i2c_master_init());
	ESP_LOGI(TAG,"i2c master initialized");
	/*
	result = i2c_master_init();
	if (result != ESP_OK)
		{
			ESP_LOGI(TAG, "i2c_master_init Initialization Error");
			return result;
		}

	ESP_LOGI(TAG, "i2c_master_and_init Initialization OK");
	*/

return result;
}


uint8_t sysinit2(void)
{
	uint8_t result = ESP_FAIL;
	//Allow other core to finish initialization
	//vTaskDelay(pdMS_TO_TICKS(100));

	ESP_ERROR_CHECK(esp_register_shutdown_handler(AppShutDownHandler));

	/*Initialize BlueFI*/
	blufi_main();

	//appwifi_init();

	result = ESP_OK;

return result;
}

uint8_t appinit1(void)
{
	uint8_t result = 0;
	/* Configure the peripheral according to the LED type */
	configure_led();

	return result;
}

uint8_t appinit2(void)
{

	apptask_init();

	consola_Init();

	uint8_t result = 0;

	return result;
}

