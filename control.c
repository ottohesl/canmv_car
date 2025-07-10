#include"go.h"
#include"main.h"
float delta_t=0.01;//控制周期
float wheel_circumference = 20.42; // cm
float speed_A=0,speed_B=0;
int output_max = 100;
float left_v=0,right_v=0;//差速比左右轮速度
float v_base=30;
int32_t prev_count_A=0,prev_count_B=0;
//转向环(pd)
float pid_turn(float target,float need){
	static float last_error=0;
	float error=need-target;
	float output=turn_kp*error+turn_kd*(error-last_error);
	last_error=error;
	return output;//输出的是单向转向速度
}
//速度环(pid)
float pid_speed(float current_speed,float need_speed){
	static float integral = 0,last_error=0;
	float error=need_speed-current_speed;
	integral+=error;
	if(integral>output_max)integral=output_max;
	else if(integral<-output_max)integral=-output_max;
	float output=speed_kp*error+speed_ki*integral+speed_kd*(error-last_error);
	last_error=error;
	return output;
}
//速度转换为差速比
void pid_output(float d_speed){
	float delta_v = d_speed;
    left_v = v_base + delta_v;
    right_v = v_base - delta_v;
    //限幅
    if(left_v>100)left_v=100;
    if(left_v<-100)left_v=-100;
    if(right_v>100)right_v=100;
    if(right_v<-100)right_v=-100;
}
void get_now_speed(){
    // 1. 读取当前编码器值
    int32_t current_count_A = __HAL_TIM_GET_COUNTER(&htim2);
    int32_t current_count_B = __HAL_TIM_GET_COUNTER(&htim3);

    // 2. 计算脉冲差值（处理溢出）
    int16_t delta_A = (int16_t)(current_count_A - prev_count_A);
    int16_t delta_B = (int16_t)(current_count_B - prev_count_B);

    // 3. 更新前值
    prev_count_A = current_count_A;
    prev_count_B = current_count_B;

    // 4. 计算线速度（cm/s）
    speed_A = (delta_A * wheel_circumference) / (2496 * delta_t);
    speed_B = (delta_B * wheel_circumference) / (2496 * delta_t);

}
