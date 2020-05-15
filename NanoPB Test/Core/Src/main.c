/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "pb_encode.h"
#include "pb_decode.h"
#include "krpc.pb.h"
#include "esp8266.h"
#include "task_list.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
		void *const*arg);
bool decode_values(pb_istream_t *stream, const pb_field_t *field, void **arg);

typedef struct {
	uint8_t *message_string;
	// include other fields as I need them
	uint8_t message_len;
} string_context_t;

typedef struct {
	krpc_schema_ProcedureCall *calls;
	// include other fields as I need them
	uint8_t numCalls;
} proccall_context_t;

typedef struct {
	uint32_t service_id;
	uint32_t procedure_id;
} procserv_context_t;

typedef struct {
	void *valueDest;
	pb_msgdesc_t *valueField;
} procval_context_t;

typedef struct {
	void **valueDests;
	krpc_schema_ProcedureResult *resultDests;
	pb_msgdesc_t **resultFields;
	uint32_t numResults;
} procresult_context_t;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

uint8_t receive_buf[30];
string_context_t decode_ctx =
		{ .message_len = 0, .message_string = receive_buf };

pb_callback_t decode_string_callback = { .funcs = { &decode_string }, .arg =
		&decode_ctx };

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

bool encode_string(pb_ostream_t *stream, const pb_field_t *field,
		void *const*arg) {
	string_context_t *ctx = (string_context_t*) *arg;
	uint8_t *str = ctx->message_string;

	if (!pb_encode_tag_for_field(stream, field))
		return false;

	return pb_encode_string(stream, str, strlen((char*) str));
}

bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	string_context_t *ctx = (string_context_t*) *arg;
	uint8_t *str = ctx->message_string;
	// read until we read a null character
	while (true) {
		if (!pb_read(stream, str + ctx->message_len, 1)) {
			return false;
		}
		if (str[ctx->message_len] == '\0') {
			ctx->message_len++;
			break;
		}
		ctx->message_len++;
	}
	return true;
}

bool encode_calls(pb_ostream_t *stream, const pb_field_t *field,
		void *const*arg) {
	proccall_context_t *calls = (proccall_context_t*) *arg;

	if (!pb_encode_tag_for_field(stream, field))
		return false;
	int i;
	bool result = false;
	for (i = 0; i < calls->numCalls; i++) {
		result = pb_encode_submessage(stream, krpc_schema_ProcedureCall_fields,
				&calls->calls[i]);
		if (!result)
			return false;
	}
	return true;
}

bool decode_results(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	procresult_context_t *results = (procresult_context_t*) *arg;
	procval_context_t value;
	int i;
	bool result = false;
	for (i = 0; i < results->numResults; i++) {
		value.valueDest = results->valueDests[i];
		value.valueField = results->resultFields[i];
		results->resultDests[i].value.funcs.decode = &decode_values;
		results->resultDests[i].value.arg = &value;
		result = pb_decode(stream, krpc_schema_ProcedureResult_fields,
				&results->resultDests[i]);
		if (!result)
			return false;
	}
	return true;
}

bool decode_values(pb_istream_t *stream, const pb_field_t *field, void **arg) {
	procval_context_t *value = (procval_context_t*) *arg;
	return pb_decode(stream, value->valueField, value->valueDest);
}

bool decode_error(pb_istream_t *stream, const pb_field_t *field,
		void *const*arg) {
	return false;
}

bool read(pb_istream_t *stream, uint8_t *buf, size_t count) {
	UART_HandleTypeDef *huart = (UART_HandleTypeDef*) stream->state;
	HAL_StatusTypeDef result;
	if ((result = HAL_UART_Receive(huart, buf, count, 5000)) != HAL_OK) {
		return false;
	}
	return true;
}

bool frame_dummy() {
	return false;
}

bool krpc_connect(char *clientName) {
	krpc_schema_ConnectionRequest mymessage;
	string_context_t ctx;
	ctx.message_string = (uint8_t*) clientName;
	mymessage.client_name.arg = &ctx;
	mymessage.client_name.funcs.encode = &encode_string;
	mymessage.type = krpc_schema_ConnectionRequest_Type_RPC;
	mymessage.client_identifier.funcs.encode = 0;
	uint8_t buffer[30];
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	pb_encode_delimited(&stream, krpc_schema_ConnectionRequest_fields,
			&mymessage);

	while (HAL_UART_Transmit_DMA(&huart2, buffer, stream.bytes_written)
			!= HAL_OK)
		;
	//HAL_Delay(50);

	// Receive response message
	krpc_schema_ConnectionResponse connRe;
	connRe.message = decode_string_callback;
	connRe.client_identifier.funcs.decode = 0;

	pb_istream_t uartIn = { &read, &huart2, 30 };
	uint64_t size = 0;
	pb_decode_varint(&uartIn, &size);
	if (!read(&uartIn, buffer, size)) {
		while (1)
			;
	}
	uartIn = pb_istream_from_buffer(buffer, size);
	pb_decode(&uartIn, krpc_schema_ConnectionResponse_fields, &connRe);
	return true;

}

bool krpc_build_calls(procserv_context_t *contexts, uint8_t numContexts,
		krpc_schema_ProcedureCall *calls) {
	int i = 0;
	for (i = 0; i < numContexts; i++) {
		string_context_t service = { (uint8_t*) "KRPC", 4 };
		string_context_t procedure = { (uint8_t*) "GetStatus", 9 };
		calls[i].procedure.arg = &procedure;
		calls[i].procedure.funcs.encode = &encode_string;
		calls[i].service.arg = &service;
		calls[i].service.funcs.encode = &encode_string;
		calls[i].procedure_id = contexts[0].procedure_id;
		calls[i].service_id = contexts[0].service_id;
		calls[i].arguments.arg = 0;
		calls[i].arguments.funcs.encode = 0;
	}
	return true;
}

bool krpc_invoke(krpc_schema_Request *request, proccall_context_t *calls) {
	request->calls.arg = calls;
	request->calls.funcs.encode = &encode_calls;
	char buffer[40];
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	pb_encode_delimited(&stream, krpc_schema_Request_fields, request);
	while (HAL_UART_Transmit_DMA(&huart2, buffer, stream.bytes_written)
			!= HAL_OK)
		;
	HAL_Delay(20);
	return true;
}

bool krpc_response(krpc_schema_Response *response,
	procresult_context_t *results) {
	pb_istream_t uartIn = { &read, &huart2, 30 };
	size_t size = 0;
	size_t buf_size = 0;
	uint8_t buffer[80];
	response->results.funcs.decode = &decode_results;
	response->results.arg = results;
	response->error.service.funcs.decode = 0;
	response->error.name.funcs.decode = 0;
	response->error.description.funcs.decode = 0;


	pb_decode_varint(&uartIn, &size);
	buf_size = size;
	if (!read(&uartIn, buffer, size)) {
		while (1)
			;
	}
	uartIn = pb_istream_from_buffer(buffer, buf_size);
	return pb_decode(&uartIn, krpc_schema_Response_fields, response);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */

	//HAL_Delay(1000);
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM3_Init();
	MX_USART2_UART_Init();
	MX_USB_DEVICE_Init();
	/* USER CODE BEGIN 2 */

	HAL_Delay(1000);

	if (esp8266_init(&huart2)) {
		while (1)
			;
	}
	if (esp8266_connect_WiFi(&huart2, "NETGEAR21", "precioussquirrel044")) {
		while (1)
			;
	}
	if (esp8266_connect_TCP(&huart2, "98.223.235.63", 50000)) {
		while (1)
			;
	}

	// Connect to server
	krpc_connect("hellloooo");

	HAL_Delay(1000);

	/*// Send a get_Status request
	 krpc_schema_Request request;
	 krpc_schema_ProcedureCall calls[2];
	 proccall_context_t callCtx;
	 procserv_context_t contexts[2] = { { 1, 3 } };
	 callCtx.calls = calls;
	 callCtx.numCalls = 1;
	 krpc_build_calls(contexts, 1, calls);
	 krpc_invoke(&request, &callCtx);*/

	// Send a get_Status request
	krpc_schema_Request request;
	krpc_schema_ProcedureCall calls[2];
	proccall_context_t callCtx;
	procserv_context_t contexts[2] = { { 1, 3 } };
	callCtx.calls = calls;
	callCtx.numCalls = 1;
	krpc_build_calls(contexts, 1, calls);
	krpc_invoke(&request, &callCtx);

	// receive status response
	krpc_schema_Response response;

	// store the fields in the proper context
	procresult_context_t resultCtx;
	resultCtx.numResults = 1;

	krpc_schema_ProcedureResult results[1];
	resultCtx.resultDests = results;

	krpc_schema_Status status;
	status.version.funcs.decode = 0;
	//status.version = decode_string_callback;
	void *valueDests[1] = { &status };
	resultCtx.valueDests = valueDests;

	pb_msgdesc_t *valueFields[1] = { krpc_schema_Status_fields };
	resultCtx.resultFields = valueFields;

	krpc_response(&response, &resultCtx);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 40;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2
			| RCC_PERIPHCLK_USB;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
	PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
	PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
	PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
	PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
	PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 0;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 74480;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/** 
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel7_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

	/*Configure GPIO pin : PA1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
