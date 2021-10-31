/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	PLAY, STOP
} Modes;

typedef enum
{
	ALL_BLINKS, ROUND_BLINK_LEFT, ROUND_BLINK_RIGHT, RUNNING_LIGHTS_LEFT, RUNNING_LIGHTS_RIGHT, PAIR_BLINKING
} Blink_patterns;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile Modes mode = PLAY;
volatile Blink_patterns blink_pattern = ALL_BLINKS;
volatile uint32_t timeout = 100;

enum
{
	LEFT, RIGHT
} direction;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void LED_All_Control(GPIO_PinState state)
{
	if (mode == PLAY)
	{
		HAL_GPIO_WritePin(GPIOD,
		LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin, state);
		HAL_Delay(timeout << 2);
	}
	return;
}

void LED_All_Blinks()
{
	HAL_GPIO_WritePin(GPIOD,
	LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin, GPIO_PIN_RESET);
	while (blink_pattern == ALL_BLINKS && mode == PLAY)
	{
		HAL_GPIO_WritePin(GPIOD,
		LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin, GPIO_PIN_SET);
		HAL_Delay(timeout);
		HAL_GPIO_WritePin(GPIOD,
		LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin, GPIO_PIN_RESET);
		HAL_Delay(timeout);
	}
}

void LED_Round_Blink()
{
	uint32_t Led_port = (direction == LEFT) ? 0x8000 : 0x1000;
	while (blink_pattern == ROUND_BLINK_LEFT && mode == PLAY)
	{
		HAL_GPIO_TogglePin(GPIOD, Led_port);
		Led_port = Led_port >> 1; //Switch  pins state one by one
		Led_port = (Led_port >= 0x1000) ? Led_port : 0x8000;
		HAL_Delay(timeout);
	}

	while (blink_pattern == ROUND_BLINK_RIGHT && mode == PLAY)
	{
		HAL_GPIO_TogglePin(GPIOD, Led_port);
		Led_port = Led_port << 1; //Switch  pins state one by one
		Led_port = (Led_port <= 0x8000) ? Led_port : 0x1000;
		HAL_Delay(timeout);
	}
}
void LED_Running_Lights()
{
	uint32_t Led_port = (direction == LEFT) ? 0x8000 : 0x1000;
	while (blink_pattern == RUNNING_LIGHTS_LEFT && mode == PLAY)
	{
		HAL_GPIO_WritePin(GPIOD, Led_port, GPIO_PIN_SET);
		HAL_Delay(timeout);
		HAL_GPIO_WritePin(GPIOD, Led_port, GPIO_PIN_RESET);
		Led_port = Led_port >> 1; //Switch  pins state one by one
		Led_port = (Led_port >= 0x1000) ? Led_port : 0x8000;
	}
	while (blink_pattern == RUNNING_LIGHTS_RIGHT && mode == PLAY)
	{
		HAL_GPIO_WritePin(GPIOD, Led_port, GPIO_PIN_SET);
		HAL_Delay(timeout);
		HAL_GPIO_WritePin(GPIOD, Led_port, GPIO_PIN_RESET);
		Led_port = Led_port << 1; //Switch  pins state one by one
		Led_port = (Led_port <= 0x8000) ? Led_port : 0x1000;
	}
}

void LED_Pair_Blinking()
{
	uint32_t count = 0;
	while (blink_pattern == PAIR_BLINKING && mode == PLAY)
	{
		if (count % 2 == 0)
		{
			HAL_GPIO_WritePin(GPIOD, LED_GREEN_Pin | LED_RED_Pin, GPIO_PIN_SET);
			HAL_Delay(timeout);
			HAL_GPIO_WritePin(GPIOD, LED_GREEN_Pin | LED_RED_Pin, GPIO_PIN_RESET);
			count++;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOD, LED_ORANGE_Pin | LED_BLUE_Pin, GPIO_PIN_SET);
			HAL_Delay(timeout);
			HAL_GPIO_WritePin(GPIOD, LED_ORANGE_Pin | LED_BLUE_Pin, GPIO_PIN_RESET);
			count = 0;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case SWT2_Pin:
	{
		mode = (mode == PLAY) ? STOP : PLAY;
	}
		break;
	case SWT1_Pin:
	{
		timeout = (timeout == 2000) ? timeout : (timeout + 10);
	}
		break;
	case SWT3_Pin:
	{
		timeout = (timeout == 10) ? timeout : (timeout - 10);
	}
		break;
	case SWT4_Pin:
	{
		blink_pattern = (blink_pattern == PAIR_BLINKING) ? ALL_BLINKS : (blink_pattern + 1);
	}
		break;
	case SWT5_Pin:
	{
		blink_pattern = (blink_pattern == ALL_BLINKS) ? PAIR_BLINKING : (blink_pattern - 1);
	}
		break;
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
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
	MX_GPIO_Init();
	/* USER CODE BEGIN 2 */
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		LED_All_Blinks();
		LED_Round_Blink();
		LED_Running_Lights();
		LED_Pair_Blinking();
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : LED_GREEN_Pin LED_ORANGE_Pin LED_RED_Pin LED_BLUE_Pin */
	GPIO_InitStruct.Pin = LED_GREEN_Pin | LED_ORANGE_Pin | LED_RED_Pin | LED_BLUE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : SWT4_Pin SWT5_Pin SWT3_Pin SWT1_Pin */
	GPIO_InitStruct.Pin = SWT4_Pin | SWT5_Pin | SWT3_Pin | SWT1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : SWT2_Pin */
	GPIO_InitStruct.Pin = SWT2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SWT2_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
