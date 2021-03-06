#include "string.h"
#include "stdio.h"
#include "esp8266.h"
#include "stm32l4xx.h"
#include "stm32l476g_discovery.h"

static uint8_t esp_buffer[ESP_BUF_SIZE];

/*-- Checks to see if the ESP8266 module is alive, then disables command echoing, and enables station mode --*/
ESP8266_STATUS esp8266_init(UART_HandleTypeDef *huart) {
	if (esp8266_reset(huart)) {
		return ESP8266_ERROR;
	}

	// Check if alive, note that echoing has yet to be disabled
	strcpy((char *) esp_buffer, C_AT);
	HAL_UART_Transmit(huart, esp_buffer, C_AT_LEN, DELAY_5_SEC);
	HAL_UART_Receive(huart, esp_buffer, C_AT_LEN + R_OK_LEN,
	DELAY_5_SEC);
	if (strncmp((char *) esp_buffer, C_AT, C_AT_LEN)
			| strncmp((char *) &esp_buffer[C_AT_LEN], R_OK, R_OK_LEN)) {
		return ESP8266_ERROR;
	}

	// Disable echoing
	strcpy((char *) esp_buffer, C_ATE("0"));
	HAL_UART_Transmit(huart, esp_buffer, C_ATE_LEN("0"), DELAY_5_SEC);
	HAL_UART_Receive(huart, esp_buffer, C_ATE_LEN("0") + R_OK_LEN,
	DELAY_5_SEC);
	if (strncmp((char *) esp_buffer, C_ATE("0"), C_ATE_LEN("0"))
			| strncmp((char *) &esp_buffer[C_ATE_LEN("0")], R_OK, R_OK_LEN)) {
		return ESP8266_ERROR;
	}

	// Enable station mode
	strcpy((char *) esp_buffer, C_AT_CWMODE("1"));
	HAL_UART_Transmit(huart, esp_buffer, C_AT_CWMODE_LEN("1"),
	DELAY_5_SEC);
	HAL_UART_Receive(huart, esp_buffer, R_OK_LEN,
	DELAY_10_SEC);
	if (strncmp((char *) esp_buffer, R_OK, R_OK_LEN)) {
		return ESP8266_ERROR;
	}

	return ESP8266_OK;
}

/*-- Physically resets the ESP8266 by triggering its reset pin using GPIOA Pin 1 --*/
ESP8266_STATUS esp8266_reset(UART_HandleTypeDef *huart) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

	HAL_UART_Receive(huart, esp_buffer, 565, DELAY_5_SEC);
	if (strncmp((char *) &esp_buffer[565 - R_RESET_LEN], R_RESET,
	R_RESET_LEN)) {
		return ESP8266_ERROR;
	}
	return ESP8266_OK;
}

/*-- Connects to WiFI network with given SSID and PSWD --*/
ESP8266_STATUS esp8266_connect_WiFi(UART_HandleTypeDef *huart, char *ssid,
		char *pswd) {
	// Create connection string
	strcpy((char *) esp_buffer, "AT+CWJAP_CUR=\"");
	strcat((char *) esp_buffer, ssid);
	strcat((char *) esp_buffer, "\",\"");
	strcat((char *) esp_buffer, pswd);
	strcat((char *) esp_buffer, "\"\r\n");

	// Connect to WiFi
	HAL_UART_Transmit(huart, esp_buffer, strlen((char *) esp_buffer),
	DELAY_5_SEC);
	HAL_UART_Receive(huart, esp_buffer, R_AT_CWJAP_LEN,
	DELAY_20_SEC);
	if (strncmp((char *) esp_buffer, R_AT_CWJAP, R_AT_CWJAP_LEN)) {
		return ESP8266_ERROR;
	}

	return ESP8266_OK;
}

/*-- Connects to TCP port specified by IP and port, sets SSL size to 4096, and enables UART pass-through --*/
ESP8266_STATUS esp8266_connect_TCP(UART_HandleTypeDef *huart, char *ip,
		uint16_t port) {
	// Convert port integer to a string
	char port_str[6];
	sprintf(port_str, "%d", port);

	// Create connection string
	strcpy((char *) esp_buffer, "AT+CIPSTART=\"TCP\",\"");
	strcat((char *) esp_buffer, ip);
	strcat((char *) esp_buffer, "\",");
	strcat((char *) esp_buffer, port_str);
	strcat((char *) esp_buffer, "\r\n");

	// Connect to TCP port
	HAL_UART_Transmit(huart, esp_buffer, strlen((char *) esp_buffer),
	DELAY_5_SEC);
	HAL_UART_Receive(huart, esp_buffer, R_AT_CIPSTART_LEN,
	DELAY_10_SEC);
	if (strncmp((char *) esp_buffer, R_AT_CIPSTART, R_AT_CIPSTART_LEN)) {
		return ESP8266_ERROR;
	}

	// Set SSL size to 4096
	strcpy((char *) esp_buffer, C_AT_CIPSSLSIZE("4096"));
	HAL_UART_Transmit(huart, esp_buffer, C_AT_CIPSSLSIZE_LEN("4096"),
	DELAY_5_SEC);
	HAL_UART_Receive(huart, esp_buffer, R_OK_LEN,
	DELAY_10_SEC);
	if (strncmp((char *) esp_buffer, R_OK, R_OK_LEN)) {
		return ESP8266_ERROR;
	}

	// Set IP mode to 1, UART pass-through
	if (esp8266_enable_passthrough(huart)) {
		return ESP8266_ERROR;
	}

	// Start communications
	strcpy((char *) esp_buffer, C_AT_CIPSEND);
	HAL_UART_Transmit(huart, esp_buffer, C_AT_CIPSEND_LEN,
	HAL_MAX_DELAY);
	HAL_UART_Receive(huart, esp_buffer, R_AT_CIPSEND_LEN,
	HAL_MAX_DELAY);
	if (strncmp((char *) esp_buffer, R_AT_CIPSEND, R_AT_CIPSEND_LEN)) {
		return ESP8266_ERROR;
	}

	return ESP8266_OK;
}

ESP8266_STATUS esp8266_enable_passthrough(UART_HandleTypeDef *huart) {
	// Set IP mode to 1, UART pass-through
	strcpy((char *) esp_buffer, C_AT_CIPMODE("1"));
	HAL_UART_Transmit(huart, esp_buffer, C_AT_CIPMODE_LEN("1"),
	DELAY_5_SEC);
	HAL_UART_Receive(huart, esp_buffer, R_OK_LEN,
	DELAY_10_SEC);
	if (strncmp((char *) esp_buffer, R_OK, R_OK_LEN)) {
		return ESP8266_ERROR;
	}

	return ESP8266_OK;
}

ESP8266_STATUS esp8266_disable_passthrough(UART_HandleTypeDef *huart) {
	// Disable UART pass-through
	strcpy((char *) esp_buffer, "+++");
	HAL_UART_Transmit(huart, esp_buffer, strlen((char *) esp_buffer),
			DELAY_5_SEC);

	return ESP8266_OK;
}
