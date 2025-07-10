/*
 * go.h
 *
 *  Created on: Apr 15, 2025
 *      Author: 34969
 */

#ifndef INC_GO_H_
#define INC_GO_H_
#include"main.h"
#include"tim.h"
void go_forward_a(uint16_t pwm);
void go_forward_b(uint16_t pwm);
void go_backward_a(uint16_t pwm);
void go_backward_b(uint16_t pwm);
void go_stop();
#endif /* INC_GO_H_ */
