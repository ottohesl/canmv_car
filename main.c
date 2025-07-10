/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"control.h"
#include"go.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
float speed_kp=4,speed_ki=0.1,speed_kd=0.8;
float turn_kp=0.3,turn_kd=0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char a[100];//上位机用数组
uint8_t rx_buf[2];  // 2字节缓冲区
float angle = 0.0f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if(huart==&huart3){
        // 1. 查找结束符'#'的位置
        char *end_ptr = memchr(a, '#', Size);

        if (end_ptr != NULL) {
            // 2. 在'#'位置添加字符串终止符
            *end_ptr = '\0';

            // 3. 解析参数（格式：up_kp,up_kd#）
            float new_kp = 0, new_kd = 0;
            int parsed = sscanf(a, "%f,%f,%f", &new_kp, &new_kd);

            // 4. 校验并更新参数
            if (parsed == 2) { // 确保解析到两个参数
            	turn_kp = new_kp;
            	turn_kd = new_kd;
            	//speed_kd = new_kd;
			}
		   }
		}

		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, a, sizeof(a));
	}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim==&htim4){
		get_now_speed();
		HAL_UART_Receive_DMA(&huart2, rx_buf, 2);
        // 解析角度值（小端模式）
        int16_t angle_int = (rx_buf[1] << 8) | rx_buf[0];
        angle = -angle_int / 100.0f;
        //转向环
		float turn=pid_turn(0,angle);
		pid_output(turn);
		//速度环
		float pwm_a=pid_speed(speed_A, left_v);
		float pwm_b=pid_speed(speed_B, right_v);
		//pwm极性判断并赋给电机
		if(pwm_a>0)go_forward_a(pwm_a);
		else if(pwm_a<0)go_backward_a(-pwm_a);
		if(pwm_b>0)go_forward_b(pwm_b);
		else if(pwm_b<0)go_backward_b(-pwm_b);

		char message[80];
		int len = sprintf(message, "%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f\n",speed_A,speed_B,pwm_a,pwm_b,angle,left_v,right_v);
		HAL_UART_Transmit_DMA(&huart3, (uint8_t*)message, len);
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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL); // 电机A编码器
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL); // 电机B编码器
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 电机A AIN1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // 电机A AIN2
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // 电机B BIN1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // 电机B BIN2
  HAL_TIM_Base_Start_IT(&htim4);

  //开启串口的上位机传输
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, a, sizeof(a));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
