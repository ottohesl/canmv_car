#include"main.h"
#include"tim.h"
#include"control.h"
void go_backward_a(uint16_t pwm){
	 //左轮--a
	 //后退为an1
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	  //PINA8-1--AN1/PINA9-0--AN2
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm);
 }
void go_backward_b(uint16_t pwm){
	  //右轮--b
	  //后退为bn1
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
	  //PINA10-1--BN1/PINA11-0--BN2
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pwm);
}
 void go_forward_a(uint16_t pwm){
	 //左轮--a
	 //前进为an2
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	  //PINA8-0--AN1/PINA9-1--AN2
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pwm);

 }
 void go_forward_b(uint16_t pwm){
	  //右轮--b
	  //前进为bn2
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
	  //PINA10-0--BN1/PINA11-1--BN2
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pwm);
 }
 void go_stop(){
	 //左轮
	    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3); // 停止BIN1的PWM
	    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4); // 停止BIN2的PWM
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET); // BIN1=1
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); // BIN2=1
	 //右轮
	    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // 停止AIN1的PWM
	    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2); // 停止AIN2的PWM
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET); // AIN1=1
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET); // AIN2=1
 }
