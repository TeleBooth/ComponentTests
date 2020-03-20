/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "stm32l4xx.h"
#include "stm32l476g_discovery.h"

/** @addtogroup STM32L4xx_HAL_Examples
 * @{
 */

/** @addtogroup Templates
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef s_UARTHandle;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void) {

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

	/* Configure the System clock to have a frequency of 4 MHz */
	SystemClock_Config();

	/* UART testing begin here */
	__USART2_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStructure;

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
	s_UARTHandle.Init.BaudRate = 57600;
	s_UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
	s_UARTHandle.Init.StopBits = UART_STOPBITS_1;
	s_UARTHandle.Init.Parity = UART_PARITY_NONE;
	s_UARTHandle.Init.HwFlowCtl = UART_HWCONTROL_RTS;
	s_UARTHandle.Init.Mode = UART_MODE_TX_RX;

	if (HAL_UART_Init(&s_UARTHandle) != HAL_OK) asm("bkpt 255");

	/*while (1) {
		char buffer[1024] = "test\r\n";
		HAL_UART_Receive(&s_UARTHandle, buffer, sizeof(buffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&s_UARTHandle, buffer, sizeof(buffer), HAL_MAX_DELAY);
	}*/

	//uint8_t recv_buffer[1024] = {0};

	uint8_t msg[] 	= "AT+CIPSTATUS\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg, sizeof(msg), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t msg0[] 	= "AT+CWMODE_CUR=1\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg0, sizeof(msg0), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t msg1[]		= "AT+CWJAP_CUR=\"Fios-7GP6L\",\"nab72vote6336push\"\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg1, sizeof(msg1), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t msg2[]		= "AT+CIPSTART=\"TCP\",\"192.168.1.158\",10000\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg2, sizeof(msg2), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t msg3[]		= "AT+CIPSSLSIZE=4096\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg3, sizeof(msg3), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t msg4[]		= "AT+CIPMODE=1\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg4, sizeof(msg4), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t msg5[]		= "AT+CIPSEND\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg5, sizeof(msg5), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t msg6[]		= "IT IS I, PABLO\r\n";
	HAL_UART_Transmit(&s_UARTHandle, msg6, sizeof(msg6), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	uint8_t exit[] = {'+', '+', '+'};
	HAL_UART_Transmit(&s_UARTHandle, exit, sizeof(exit), HAL_MAX_DELAY);
	//HAL_UART_Receive(&s_UARTHandle, recv_buffer, sizeof(recv_buffer), HAL_MAX_DELAY);

	HAL_UART_Transmit(&s_UARTHandle, msg, sizeof(msg), HAL_MAX_DELAY);

	/* UART testing end here */

	/* Infinite loop */
	while (1) {
	}
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follows :
 *            System Clock source            = MSI
 *            SYSCLK(Hz)                     = 4000000
 *            HCLK(Hz)                       = 4000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 4000000
 *            Flash Latency(WS)              = 0
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };

	/* The following clock configuration sets the Clock configuration sets after System reset                */
	/* It could be avoided but it is kept to illustrate the use of HAL_RCC_OscConfig and HAL_RCC_ClockConfig */
	/* and to be eventually adapted to new clock configuration                                               */

	/* MSI is enabled after System reset at 4Mhz, PLL not used */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		/* Initialization Error */
		while (1)
			;
	}

	/* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
	/* Set 0 Wait State flash latency for 4Mhz */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		/* Initialization Error */
		while (1)
			;
	}

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE()
	;

	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2)
			!= HAL_OK) {
		/* Initialization Error */
		while (1)
			;
	}

	/* Disable Power Control clock */
	__HAL_RCC_PWR_CLK_DISABLE();
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
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
