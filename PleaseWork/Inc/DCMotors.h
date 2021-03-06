/*
 * DCMotors.h
 *
 *  Created on: 7 de dez de 2018
 *      Author: esteves
 */

#ifndef DCMOTORS_H_
#define DCMOTORS_H_

#include "DCMotors.h"
#include "stm32f0xx_hal.h"

#define DELTA_COL_LIN 		1021
#define DELTA_CHAR_H 		1016
#define DELTA_CHAR_V 		1027

#define MAX_POINT 			1096

#define P_FRACTION 			1.0     //Proportional factor of control loop 0.001 - 10.0 (1.0)
#define STEP_MARGIN 		10     //10 - 1000 (1)

#define MIN_DUTYCYCLE 		175   //0 - 255 (125)
#define MAX_DUTYCYCLE 		255  //0 - 255 (255)

#define PIERCE_TIME			100

typedef struct{
	signed long setPoint;
	uint32_t actualPoint;
	uint8_t stepStatusOld;

	GPIO_TypeDef* encoder_CE_port;
	uint16_t encoder_CE_pin;

	TIM_HandleTypeDef timer_left;
	TIM_HandleTypeDef timer_right;

	uint16_t channel_left;
	uint16_t channel_right;

	uint8_t dutyCycle;
}MotorControl_t;

typedef struct{
	GPIO_TypeDef* A_PORT;
	uint16_t A_PIN;

	GPIO_TypeDef* B_PORT;
	uint16_t B_PIN;
}MotorControl_Simple_t;

void motorBegin(MotorControl_t* motor, GPIO_TypeDef* encoder_CE_port, uint16_t encoder_CE_pin, TIM_HandleTypeDef timer_left, TIM_HandleTypeDef timer_right, uint16_t channel_left, uint16_t channel_right);
void atualizarEixo(MotorControl_t* motor,signed long setPoint);
void motorBackward(MotorControl_Simple_t* motor, uint32_t time);
void motorForward(MotorControl_Simple_t* motor, uint32_t time);
void motorSimpleBegin(MotorControl_Simple_t* motor, GPIO_TypeDef* a_port, uint16_t a_pin, GPIO_TypeDef* b_port, uint16_t b_pin);
//void pierce(MotorControl_Simple_t* motor, uint32_t time);
double myABS(double num1);

#endif /* DCMOTORS_H_ */
