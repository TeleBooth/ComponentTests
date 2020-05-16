#include "string.h"
#include "stdio.h"
#include "esp8266.h"
//#include "stm32l476g_discovery.h"

static uint8_t esp_buffer[ESP_BUF_SIZE];

// this will let us take the next "responseSize" bytes received and put them aside as our response
extern uint8_t responseBuffer[1024];
uint16_t responseSizeTmp = 0;
extern int16_t responseSize;
extern uint8_t UserTxBufferFS[2048];
uint8_t resetSeq = 0;
uint8_t passthroughMode = 0;

// transmit and receive wrappers used to implement the response behavior
void Transmit_Wrapper(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint16_t responseSize){
	responseSizeTmp = responseSize;
	while(HAL_UART_Transmit_DMA(huart, pData, Size) == HAL_BUSY);
	HAL_Delay(500);
}

// busy waits
ESP8266_STATUS Receive_Wrapper(uint8_t * responseBufferSmol, const char * compareString, uint16_t compareStringLength){
	while(responseSize > 0);
	if (strncmp((char *) responseBufferSmol, compareString, compareStringLength)) {
		return ESP8266_ERROR;
		}
	return ESP8266_OK;
}

/*-- Checks to see if the ESP8266 module is alive, then disables command echoing, and enables station mode --*/
ESP8266_STATUS esp8266_init(UART_HandleTypeDef *huart) {
	if (esp8266_reset(huart)) {
		return ESP8266_ERROR;
	}

	// Check if alive, note that echoing has yet to be disabled
	strcpy((char *) esp_buffer, C_AT);
	Transmit_Wrapper(huart, esp_buffer, C_AT_LEN, C_AT_LEN + R_OK_LEN);
	if (Receive_Wrapper(responseBuffer, C_AT_OK , C_AT_LEN + R_OK_LEN)) {
		return ESP8266_ERROR;
	}
	//HAL_Delay(100);

	// Disable echoing
	strcpy((char *) esp_buffer, C_ATE("0"));
	Transmit_Wrapper(huart, esp_buffer, C_ATE_LEN("0"), C_ATE_LEN("0") + R_OK_LEN);
	if (Receive_Wrapper(responseBuffer, C_ATE0_OK , C_ATE_LEN("0") + R_OK_LEN)) {
		return ESP8266_ERROR;
	}
	//HAL_Delay(100);

	// Enable station mode
	strcpy((char *) esp_buffer, C_AT_CWMODE("1"));
	Transmit_Wrapper(huart, esp_buffer, C_AT_CWMODE_LEN("1"), R_OK_LEN);
	if(Receive_Wrapper(responseBuffer, R_OK , R_OK_LEN))
		return ESP8266_ERROR;
	//HAL_Delay(100);


	return ESP8266_OK;
}

/*-- Physically resets the ESP8266 by triggering its reset pin using GPIOA Pin 1 --*/
ESP8266_STATUS esp8266_reset(UART_HandleTypeDef *huart) {
	strcpy((char *) esp_buffer, "AT+RST\r\n");

	// this helps to deal with the uncertainty surrounding how long the reset sequence is
	resetSeq = 1;
	// IMPORTANT: YOU MIGHT NEED TO MODIFY THIS DEPENDING ON WHERE IN YOUR RESET SEQUENCE "READY" SHOWS UP
	// but if we pass
	// after transmitting and the second-long delay that elapses, hopefully we'll have it
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(500);
	resetSeq = 0;

	// we might need to iterate through a few different strings to see if we can find "ready"
	int i;
	// it seems like we might lose upwards of 15 characters, so I want to make a wide window
	// starting from 325 going to 340 as starting points to look for "ready"
	for(i = 0; i <= 15; i++){
		if(!Receive_Wrapper(&responseBuffer[325+i], "ready" , 5))
			return ESP8266_OK;
	}
	return ESP8266_ERROR;
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
	resetSeq = 1;
	// Ethier we wait for it to connect, or we wait for it not connect
	// since not connecting provides the shorter sequence, then we can look at that to not lock our
	// receiving wrapper
	Transmit_Wrapper(huart, esp_buffer, strlen((char *) esp_buffer), R_AT_CWJAP_FAIL_LEN);
	HAL_Delay(3000);
	resetSeq = 0;
	if(Receive_Wrapper(responseBuffer, R_AT_CWJAP_FAIL, R_AT_CWJAP_FAIL_LEN))
		return ESP8266_ERROR;

	return ESP8266_OK;
}

krpc_connection_t connection;
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

	Transmit_Wrapper(huart, esp_buffer, strlen((char *) esp_buffer), R_AT_CIPSTART_LEN);
	if(Receive_Wrapper(responseBuffer, R_AT_CIPSTART, R_AT_CIPSTART_LEN))
		return ESP8266_ERROR;

	// Initialize the kRPC connection
	connection.inBufPtr = UserTxBufferFS;
	connection.huart = huart;

	if(krpc_connect(&connection, "HELLO") != KRPC_OK)
		return ESP8266_ERROR;

	// Set SSL size to 4096
	strcpy((char *) esp_buffer, C_AT_CIPSSLSIZE("4096"));
	Transmit_Wrapper(huart, esp_buffer, C_AT_CIPSSLSIZE_LEN("4096"), R_OK_LEN);
	if(Receive_Wrapper(responseBuffer, R_OK, R_OK_LEN))
		return ESP8266_ERROR;


	// Set IP mode to 1, UART pass-through
	if (esp8266_enable_passthrough(huart)) {
		return ESP8266_ERROR;
	}

	// Start communications
	strcpy((char *) esp_buffer, C_AT_CIPSEND);
	Transmit_Wrapper(huart, esp_buffer, C_AT_CIPSEND_LEN, R_AT_CIPSEND_LEN);
	if(Receive_Wrapper(responseBuffer, R_AT_CIPSEND, R_AT_CIPSEND_LEN))
		return ESP8266_ERROR;

	return ESP8266_OK;
}

ESP8266_STATUS esp8266_enable_passthrough(UART_HandleTypeDef *huart) {
	// Set IP mode to 1, UART pass-through
	strcpy((char *) esp_buffer, C_AT_CIPMODE("1"));
	Transmit_Wrapper(huart, esp_buffer, C_AT_CIPMODE_LEN("1"), R_OK_LEN);
	if(Receive_Wrapper(responseBuffer, R_OK, R_OK_LEN))
		return ESP8266_ERROR;

	passthroughMode = 1;
	return ESP8266_OK;
}

ESP8266_STATUS esp8266_disable_passthrough(UART_HandleTypeDef *huart) {
	// Disable UART pass-through
	strcpy((char *) esp_buffer, "+++");
	while(HAL_UART_Transmit_DMA(huart, esp_buffer, strlen((char *) esp_buffer)) == HAL_ERROR);
	Transmit_Wrapper(huart, esp_buffer, strlen((char *) esp_buffer), 0);
	passthroughMode = 0;
	return ESP8266_OK;
}
