/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "string.h"
#include "esp8266.h"
#include "task_list.h"
#include "stm32l4xx.h"
#include "stm32l476g_discovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	BUF_SIZE 1024
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef s_UARTHandle;
static uint8_t rx_flag;
static uint8_t tx_flag;
static uint8_t buffer[BUF_SIZE + 1];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Init(void);
//static uint8_t ESP8266_Init(void);

/* Private functions ---------------------------------------------------------*/
void USART2_IRQHandler(void) {
	HAL_UART_IRQHandler(&s_UARTHandle);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (rx_flag) {
		rx_flag = 0;
		HAL_UART_Transmit_IT(huart, &buffer[BUF_SIZE], 1);
	} else {
		rx_flag = 1;
		HAL_UART_Receive_IT(huart, buffer, buffer[BUF_SIZE]);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (tx_flag) {
		tx_flag = 0;
		HAL_UART_Receive_IT(huart, &buffer[BUF_SIZE], 1);
	}
	else {
		tx_flag = 1;
		HAL_UART_Transmit_IT(huart, buffer, buffer[BUF_SIZE]);
	}
}

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {
	Init();
	if (esp8266_init(&s_UARTHandle)) {
		while (1)
			;
	}
	if (esp8266_connect_WiFi(&s_UARTHandle, "ESP8266",
			"hijason12")) {
		while (1)
			;
	}
	if (esp8266_connect_TCP(&s_UARTHandle, "172.20.10.2", 10000)) {
		while (1)
			;
	}

	// TODO: connect to the kRPC server
	// 1. port krpc-cnano into this project
	// 	1.1 writing the four functions in "communication.h"
	//	1.2 writing the iostream callback functions
	// 2. get the initialization sequence in this code

	init_task_list();

	rx_flag = 0;
	tx_flag = 0;
	strcpy((char *) buffer, "Hello Jeremy every 15 seconds!");
	buffer[BUF_SIZE] = (uint8_t) strlen((char *) buffer);
	//HAL_UART_Transmit_IT(&s_UARTHandle, &buffer[BUF_SIZE], 1);
	add_task(REQUEST_TASK, buffer[BUF_SIZE], (void *) buffer, 15 * SEC, 15 * SEC);
	strcpy((char *) buffer, "Hello Nick every 10 seconds!");
	buffer[BUF_SIZE] = (uint8_t) strlen((char *) buffer);
	add_task(REQUEST_TASK, buffer[BUF_SIZE], (void *) buffer, 10 * SEC, 10 * SEC);
	strcpy((char *) buffer, "Hello Pablo every 5 seconds!");
	buffer[BUF_SIZE] = (uint8_t) strlen((char *) buffer);
	add_task(REQUEST_TASK, buffer[BUF_SIZE], (void *) buffer, 5 * SEC, 5 * SEC);

	/* Infinite loop */
	while (1) {
		//asm("wfi");
		handle_tasks(&s_UARTHandle);
	}
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 6;
	RCC_OscInitStruct.PLL.PLLN = 20;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		while (1)
			;
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
		while (1)
			;
	}

	__HAL_RCC_PWR_CLK_ENABLE()
	;

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		while (1)
			;
	}
}

void Init(void) {
	/* STM32L4xx HAL library initialization:
	 - Configure the Flash prefetch, Flash preread and Buffer caches
	 - Systick timer is configured by default as source of time base, but user
	 can eventually implement his proper time base source (a general purpose
	 timer for example or other time source), keeping in mind that Time base
	 duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
	 handled in milliseconds basis.
	 - Low Level Initialization
	 */
	HAL_Init();

	/* Configure the System clock to have a frequency of 80 MHz */
	SystemClock_Config();

	/* UART testing begin here */
	__USART2_CLK_ENABLE()
	;
	__GPIOA_CLK_ENABLE()
	;

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_1;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

	GPIO_InitStructure.Pin = GPIO_PIN_2;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART2;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_3;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	s_UARTHandle.Instance = USART2;
	s_UARTHandle.Init.BaudRate = 74880;
	s_UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
	s_UARTHandle.Init.StopBits = UART_STOPBITS_1;
	s_UARTHandle.Init.Parity = UART_PARITY_NONE;
	s_UARTHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	s_UARTHandle.Init.Mode = UART_MODE_TX_RX;
	s_UARTHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	s_UARTHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	s_UARTHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	if (HAL_UART_Init(&s_UARTHandle) != HAL_OK)
		asm("bkpt 255");

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	NVIC_EnableIRQ(USART2_IRQn);
}
