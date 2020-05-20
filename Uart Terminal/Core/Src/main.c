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
#include "esp8266.h"
#include "task_list.h"
#include "services/space_center.h"
#include "services/krpc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define	BUF_SIZE 1024
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;
controller_handler_context_t controlCtx;
krpc_connection_t connection;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_DMA_Init(void);
//static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	uint8_t buffer[BUF_SIZE];
	double altitude;

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
	MX_GPIO_Init();
	//MX_DMA_Init();
	//MX_USART2_UART_Init();
	MX_USB_DEVICE_Init();
	/* USER CODE BEGIN 2 */

	// this delay gives you enough time to connect the STM to realterm
	HAL_Delay(500);

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

	// Initialize the kRPC connection
	connection.huart = &huart2;
	connection.buf = buffer;
	connection.bytes_written = 0;

	if (krpc_connect(&connection, "Real Deal") != KRPC_OK) {
		while (1)
			;
	}

	//HAL_Delay(200);
	krpc_SpaceCenter_Vessel_t vessel;
	krpc_SpaceCenter_ActiveVessel(&connection, &vessel);
	//HAL_Delay(200);

	krpc_SpaceCenter_Vessel_set_Name(&connection, vessel, "Test Flight");
	//HAL_Delay(200);

	krpc_SpaceCenter_Control_t control;
	krpc_SpaceCenter_Vessel_Control(&connection, &control, vessel);
	//HAL_Delay(50);

	_Bool returnValue;
	krpc_SpaceCenter_Control_SAS(&connection, &returnValue, control);

	// store the objects we need to control the current vessel
	controlCtx.control = &control;
	controlCtx.vessel = &vessel;

	init_task_list();

	strcpy((char*) buffer, "Hello Jeremy every 15 seconds!");
	buffer[BUF_SIZE] = (uint8_t) strlen((char*) buffer);
	//HAL_UART_Transmit_IT(&s_UARTHandle, &buffer[BUF_SIZE], 1);
	add_task(RESPONSE_TASK, buffer[BUF_SIZE], (void*) buffer, 1 * SEC,
			.5 * SEC);

	add_task(REQUEST_TASK, buffer[BUF_SIZE], (void*) buffer, 1 * SEC,
				1 * SEC);

	while (1) {
		//HAL_Delay(100);
		handle_tasks(&huart2);
		//krpc_SpaceCenter_ActiveVessel(&connection, &vessel);
	}


	// TODO: connect to the kRPC server
	/**
	 * Why is the server receiving garbage?
	 * Investigate the single return byte?
	 * See why the decode process is overrunning the buffer?
	 */
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		//asm("wfi");

		handle_tasks(&huart2);
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
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */

/** 
 * Enable DMA controller clock
 */

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

// we need to arrive at these functions from the task_list because we can't #include the services and their types anywhere else
void krpc_Toggle_SAS(controller_tuple_t *currentCommand) {
	_Bool returnValue;
	krpc_SpaceCenter_Control_SAS(&connection, &returnValue,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control));
	krpc_SpaceCenter_Control_set_SAS(&connection,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control), !returnValue);
	return;
}

void krpc_Toggle_RCS(controller_tuple_t *currentCommand) {
	_Bool returnValue;
	krpc_SpaceCenter_Control_RCS(&connection, &returnValue,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control));
	krpc_SpaceCenter_Control_set_RCS(&connection,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control), !returnValue);
	return;
}

void krpc_Next_Stage(controller_tuple_t *currentCommand) {
	krpc_list_object_t vessels;

	// pre-flight
	krpc_SpaceCenter_Control_set_SAS(&connection,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control), true);
	//HAL_Delay(50);
	krpc_SpaceCenter_Control_set_RCS(&connection,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control), true);
	//HAL_Delay(50);
	krpc_SpaceCenter_Control_set_Throttle(&connection,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control), 1);

	//HAL_Delay(50);
	krpc_SpaceCenter_Control_ActivateNextStage(&connection, &vessels,
			*((krpc_SpaceCenter_Control_t*) controlCtx.control));

	// reacquire active vessel and control
	krpc_SpaceCenter_ActiveVessel(&connection,
			(krpc_SpaceCenter_Vessel_t*) controlCtx.vessel);
	krpc_SpaceCenter_Vessel_Control(&connection,
			(krpc_SpaceCenter_Control_t*) controlCtx.control,
			*(krpc_SpaceCenter_Vessel_t*) controlCtx.vessel);
	return;
}

void krpc_Pitch(controller_tuple_t *currentCommand) {
	// pitch at a constant rate of .05
	if (currentCommand->btnBuf[1] == 'U')
		krpc_SpaceCenter_Control_set_Pitch(&connection,
				*((krpc_SpaceCenter_Control_t*) controlCtx.control), .5f);
	else
		krpc_SpaceCenter_Control_set_Pitch(&connection,
				*((krpc_SpaceCenter_Control_t*) controlCtx.control), -.5f);
	return;
}

void krpc_Yaw(controller_tuple_t *currentCommand) {
	// pitch at a constant rate of .05
	if (currentCommand->btnBuf[1] == 'L')
		krpc_SpaceCenter_Control_set_Yaw(&connection,
				*((krpc_SpaceCenter_Control_t*) controlCtx.control), .5f);
	else
		krpc_SpaceCenter_Control_set_Yaw(&connection,
				*((krpc_SpaceCenter_Control_t*) controlCtx.control), -.5f);
	return;
}

void krpc_Poll(void){
	krpc_KRPC_CurrentGameScene(&connection, (krpc_KRPC_GameScene_t*) controlCtx.gameScene);
	return;
}

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
