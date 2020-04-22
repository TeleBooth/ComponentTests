#include "stm32l4xx.h"
//#include "stm32l476g_discovery.h"

/*-- Miscellaneous --*/
#define ESP_BUF_SIZE	1024
#define DELAY_5_SEC		5000
#define DELAY_10_SEC	10000
#define DELAY_20_SEC	20000

/*-- AT Commands --*/
#define C_AT							"AT\r\n"
#define	C_AT_LEN						strlen(C_AT)

#define C_AT_OK							"AT\r\n\r\nOK\r\n"

#define C_ATE(MODE)						"ATE"MODE"\r\n"
#define C_ATE_LEN(MODE)					strlen(C_ATE(MODE))

#define C_ATE0_OK						"ATE0\r\n\r\nOK\r\n"

#define C_AT_CWMODE(MODE)				"AT+CWMODE_CUR="MODE"\r\n"
#define C_AT_CWMODE_LEN(MODE)			strlen(C_AT_CWMODE(MODE))

#define C_AT_CWJAP(SSID, PSWD)			"AT+CWJAP_CUR=\""SSID"\",\""PSWD"\"\r\n"
#define C_AT_CWJAP_LEN(SSID, PSWD)		strlen(C_AT_CWJAP(SSID, PSWD))

#define C_AT_CIPSTART(IP, PORT)			"AT+CIPSTART=\"TCP\",\""IP"\","PORT"\r\n"
#define C_AT_CIPSTART_LEN(IP, PORT)		strlen(C_AT_CIPSTART(IP, PORT))

#define C_AT_CIPSSLSIZE(SIZE)			"AT+CIPSSLSIZE="SIZE"\r\n"
#define C_AT_CIPSSLSIZE_LEN(SIZE)		strlen(C_AT_CIPSSLSIZE(SIZE))

#define C_AT_CIPMODE(MODE)				"AT+CIPMODE="MODE"\r\n"
#define C_AT_CIPMODE_LEN(MODE)			strlen(C_AT_CIPMODE(MODE))

#define C_AT_CIPSEND					"AT+CIPSEND\r\n"
#define C_AT_CIPSEND_LEN				strlen(C_AT_CIPSEND)


/*-- AT Responses --*/
#define R_OK							"\r\nOK\r\n"
#define	R_OK_LEN						strlen(R_OK)

#define R_RESET							"\r\nready\r\n"
#define R_RESET_LEN						strlen(R_RESET)

#define R_AT_CWJAP						"WIFI CONNECTED\r\nWIFI GOT IP\r\n\r\nOK\r\n"
#define R_AT_CWJAP_LEN					strlen(R_AT_CWJAP)

#define R_AT_CIPSTART					"CONNECT\r\n\r\nOK\r\n"
#define R_AT_CIPSTART_LEN				strlen(R_AT_CIPSTART)

#define R_AT_CIPSEND					"\r\nOK\r\n\r\n>"
#define	R_AT_CIPSEND_LEN				strlen(R_AT_CIPSEND)

/*-- ESP8266  statuses --*/
typedef enum ESP8266_STATUS {
	ESP8266_OK		= 0,
	ESP8266_ERROR	= 1,
} ESP8266_STATUS;

/*-- esp8266.c functions --*/
ESP8266_STATUS esp8266_init (UART_HandleTypeDef *huart);
ESP8266_STATUS esp8266_reset (UART_HandleTypeDef *huart);
ESP8266_STATUS esp8266_connect_WiFi (UART_HandleTypeDef *huart, char *ssid, char *pswd);
ESP8266_STATUS esp8266_connect_TCP (UART_HandleTypeDef *huart, char *ip, uint16_t port);
ESP8266_STATUS esp8266_enable_passthrough (UART_HandleTypeDef *huart);
ESP8266_STATUS esp8266_disable_passthrough (UART_HandleTypeDef *huart);
