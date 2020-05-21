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

#include "encoders.h"
#include "decoders.h"
#include "types.h"
#include "esp8266.h"
#include "task_list.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// writing the bytes after they've been encoded
bool write(uint8_t * buf, uint8_t count) {
	HAL_StatusTypeDef result;
	while ((result = HAL_UART_Transmit(&huart2, buf, count, 200)) != HAL_OK);
	HAL_Delay(5);
	return true;
}

// reading the bytes to decode later
bool read(uint8_t * buf, size_t count) {
	HAL_StatusTypeDef result;
	result = HAL_UART_Receive(&huart2, buf, count, 2000);
	return result;
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

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

bool krpc_connect(char *clientName) {
	krpc_schema_ConnectionRequest mymessage;
	mymessage.client_name.arg = (uint8_t*) clientName;
	mymessage.client_name.funcs.encode = &encode_string;
	mymessage.type = krpc_schema_ConnectionRequest_Type_RPC;
	mymessage.client_identifier.funcs.encode = 0;
	uint8_t buffer[30];
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	pb_encode_delimited(&stream, krpc_schema_ConnectionRequest_fields,
			&mymessage);

	write(buffer, stream.bytes_written);
	//HAL_Delay(50);

	// Receive response message
	krpc_schema_ConnectionResponse connRe;
	char responseMsg[40];
	connRe.message.funcs.decode = &decode_string;
	connRe.message.arg = responseMsg;
	connRe.client_identifier.funcs.decode = 0;

	// make the stream for sizing
	size_t size = 0;
	if (read(responseMsg, 1) != HAL_OK) {
					while (1)
						;
		}
	pb_istream_t uartIn = pb_istream_from_buffer(responseMsg, 1);
	pb_decode_varint(&uartIn, &size);
	if (read(responseMsg, size) != HAL_OK) {
		while (1)
			;
	}
	// remake the stream with the real size
	uartIn = pb_istream_from_buffer(responseMsg, size);
	pb_decode(&uartIn, krpc_schema_ConnectionResponse_fields, &connRe);
	return true;

}

bool krpc_build_calls(procserv_context_t *procSerCtx, uint8_t numContexts,
		krpc_schema_ProcedureCall *calls, procarg_context_t *argCtx) {
	int i = 0;
	for (i = 0; i < numContexts; i++) {
		char emptystr[] = "";
		calls[i].procedure.arg = emptystr;
		calls[i].procedure.funcs.encode = &encode_string;
		calls[i].service.arg = emptystr;
		calls[i].service.funcs.encode = &encode_string;
		calls[i].procedure_id = procSerCtx[i].procedure_id;
		calls[i].service_id = procSerCtx[i].service_id;
		if (procSerCtx[i].hasArgs){
		calls[i].arguments.arg = &argCtx[i];
		calls[i].arguments.funcs.encode = &encode_args;
		}
		else{
			calls[i].arguments.arg = 0;
			calls[i].arguments.funcs.encode = 0;
		}
	}
	return true;
}

bool krpc_invoke(krpc_schema_Request *request, proccall_context_t *calls) {
	request->calls.arg = calls;
	request->calls.funcs.encode = &encode_calls;
	uint8_t buffer[40];
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	pb_encode_delimited(&stream, krpc_schema_Request_fields, request);
	write(buffer, stream.bytes_written);
	return true;
}

bool krpc_response(krpc_schema_Response *response,
		procresult_context_t *results) {
	response->results.funcs.decode = &decode_results;
	response->results.arg = results;
	response->error.service.funcs.decode = 0;
	response->error.name.funcs.decode = 0;
	response->error.description.funcs.decode = 0;

	uint8_t buffer[80];
	pb_istream_t uartIn = pb_istream_from_buffer(buffer, 1);
	size_t size = 0;
	if (read(buffer, 1) != HAL_OK) {
				while (1)
					;
	}
	pb_decode_varint(&uartIn, &size);
	if (read(buffer, size) != HAL_OK) {
			while (1)
				;
	}
	// remake the stream with the real size
	uartIn = pb_istream_from_buffer(buffer, size);
	pb_decode(&uartIn, krpc_schema_Response_fields, response);
	return true;
}

bool krpc_get_Status(krpc_schema_Status *status) {
	// send the request
	krpc_schema_Request request;
	krpc_schema_ProcedureCall calls[2];
	proccall_context_t callCtx;
	procarg_context_t argCtx[1];
	procserv_context_t contexts[2] = { { 1, 3, false } };
	callCtx.calls = calls;
	callCtx.numCalls = 1;
	krpc_build_calls(contexts, 1, calls, argCtx);
	krpc_invoke(&request, &callCtx);

	// get the response
	// receive status response
	krpc_schema_Response response;


	// store the fields in the proper context
	procresult_context_t resultCtx;
	resultCtx.numResults = 1;

	krpc_schema_ProcedureResult results[1];
	resultCtx.resultDests = results;

	char version_str[6];
	status->version.funcs.decode = &decode_string;
	status->version.arg = version_str;
	void *valueDests[1] = { status };
	resultCtx.valueDests = valueDests;

	pb_msgdesc_t *valueFields[1] = { krpc_schema_Status_fields };
	resultCtx.resultFields = valueFields;

	krpc_response(&response, &resultCtx);
	return true;
}

bool krpc_get_ClientName(char *client_name) {
	krpc_schema_Request request;
	krpc_schema_ProcedureCall calls[2];
	proccall_context_t callCtx;
	procarg_context_t argCtx[1];
	procserv_context_t contexts[2] = { { 1, 2, false } };
	callCtx.calls = calls;
	callCtx.numCalls = 1;
	krpc_build_calls(contexts, 1, calls, 0);
	krpc_invoke(&request, &callCtx);

	// receive client_name response
	krpc_schema_Response response;

	// store the fields in the proper context
	procresult_context_t resultCtx;
	krpc_schema_ProcedureResult results[1];
	resultCtx.numResults = 1;
	resultCtx.resultDests = results;
	void *valueDests[1] = { &client_name };
	resultCtx.valueDests = valueDests;
	pb_msgdesc_t *valueFields[1] = { STRING };
	resultCtx.resultFields = valueFields;
	krpc_response(&response, &resultCtx);
	return true;
}

/*bool krpc_set_Paused(bool *paused) {
	krpc_schema_Request request;
	krpc_schema_ProcedureCall calls[2];
	proccall_context_t callCtx;
	procarg_context_t argCtx[1];
	procserv_context_t contexts[2] = { { 1, 2, false } };
	callCtx.calls = calls;
	callCtx.numCalls = 1;
	krpc_build_calls(contexts, 1, calls, 0);
	krpc_invoke(&request, &callCtx);

	// receive client_name response
	krpc_schema_Response response;

	// store the fields in the proper context
	procresult_context_t resultCtx;
	krpc_schema_ProcedureResult results[1];
	resultCtx.numResults = 1;
	resultCtx.resultDests = results;
	void *valueDests[1] = { &client_name };
	resultCtx.valueDests = valueDests;
	pb_msgdesc_t *valueFields[1] = { STRING };
	resultCtx.resultFields = valueFields;
	krpc_response(&response, &resultCtx);
	return true;
}*/


bool krpc_get_CurrentGameScene(krpc_schema_Procedure_GameScene *game_scene) {
	krpc_schema_Request request;
	krpc_schema_ProcedureCall calls[2];
	proccall_context_t callCtx;
	procarg_context_t argCtx[1];
	procserv_context_t contexts[2] = { { 1, 11, false } };
	callCtx.calls = calls;
	callCtx.numCalls = 1;
	krpc_build_calls(contexts, 1, calls, argCtx);
	krpc_invoke(&request, &callCtx);

	// receive client_name response
	krpc_schema_Response response;

	// store the fields in the proper context
	procresult_context_t resultCtx;
	krpc_schema_ProcedureResult results[1];
	resultCtx.numResults = 1;
	resultCtx.resultDests = results;
	void *valueDests[1] = { game_scene };
	resultCtx.valueDests = valueDests;
	pb_msgdesc_t *valueFields[1] = { ENUM };
	resultCtx.resultFields = valueFields;
	krpc_response(&response, &resultCtx);
	// for some reason we need to shift it over
	*game_scene = *game_scene >> 1;
	return true;
}

bool krpc_SpaceCenter_ActiveVessel(krpc_SpaceCenter_Vessel_t * vessel){
	krpc_schema_Request request;
	krpc_schema_ProcedureCall calls[1];
	proccall_context_t callCtx;
	procarg_context_t argCtx[1];
	procserv_context_t contexts[1] = { { 2, 22, false } };
	callCtx.calls = calls;
	callCtx.numCalls = 1;
	krpc_build_calls(contexts, 1, calls, argCtx);
	krpc_invoke(&request, &callCtx);

	// receive client_name response
	krpc_schema_Response response;

	// store the fields in the proper context
	procresult_context_t resultCtx;
	krpc_schema_ProcedureResult results[1];
	resultCtx.numResults = 1;
	resultCtx.resultDests = results;
	void *valueDests[1] = { vessel };
	resultCtx.valueDests = valueDests;
	pb_msgdesc_t *valueFields[1] = { OBJECT };
	resultCtx.resultFields = valueFields;
	krpc_response(&response, &resultCtx);
	return true;
}

bool krpc_SpaceCenter_SetVesselName(krpc_SpaceCenter_Vessel_t vessel, char * name){
	krpc_schema_Request request;
	krpc_schema_ProcedureCall calls[1];
	krpc_schema_Argument vesselName;
	krpc_schema_Argument vesselInstance;
	krpc_schema_Argument *args[2];
	proccall_context_t callCtx;
	procarg_context_t argCtx[1];
	procserv_context_t contexts[1] = { { 2, 832, true } };
	callCtx.calls = calls;
	callCtx.numCalls = 1;
	vesselInstance.position = 0;
	vesselInstance.value.funcs.encode = &encode_uint64;
	vesselInstance.value.arg = &vessel;
	vesselName.position = 1;
	vesselName.value.funcs.encode = &encode_string;
	vesselName.value.arg = name;
	args[0] = &vesselInstance;
	args[1] = &vesselName;
	argCtx[0].args = args;
	argCtx[0].numArgs = 2;
	krpc_build_calls(contexts, 1, calls, argCtx);
	krpc_invoke(&request, &callCtx);
	return true;
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
	if (esp8266_connect_TCP(&huart2, "192.168.0.7", 50000)) {
		while (1)
			;
	}

	// Connect to server
	krpc_connect("hello");

	HAL_Delay(500);

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
	krpc_schema_Status status;
	krpc_get_Status(&status);

	HAL_Delay(100);

	//char client_name[40]; // the clientID bytes will be stored here
	//krpc_get_ClientName(client_name);

	krpc_schema_Procedure_GameScene game_scene;
	krpc_get_CurrentGameScene(&game_scene);

	HAL_Delay(100);

	krpc_SpaceCenter_Vessel_t vessel;
	krpc_SpaceCenter_ActiveVessel(&vessel);

	HAL_Delay(500);

	krpc_SpaceCenter_SetVesselName(vessel, "jebby");


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
