/*
 * control.h
 *
 *  Created on: Apr 16, 2025
 *      Author: 34969
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_
#include"main.h"
#include"go.h"
float pid_speed(float current_speed,float need_speed);
float pid_turn(float target,float need);
void pid_output(float d_speed);
void get_now_speed();
extern float speed_A,speed_B;
extern float left_v,right_v;
#endif /* INC_CONTROL_H_ */
