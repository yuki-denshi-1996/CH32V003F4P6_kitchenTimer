/*
 * CH32V003(CH32V003F4P6)�� �����ީ`�饤�֥��
 * Mylib_Timer.h
 *
 *  Created on: Sep 25, 2024
 *      Author: yuki(denshi-1996)
 */

#ifndef USER_MYLIB_TIMER_H_
#define USER_MYLIB_TIMER_H_

//Timer1��PWM�Ȥ���ʹ���O��
void TIM1_PWM_init(void);

//PWM��PC5(T1CH3_3)�˽ӾA
void tone(int freq); //TIM1(PWM)��ʹ�ä������Q�餹�v��
void no_tone(void); //tone���Q�餷������ֹ����v��

//Timer2���ڻ��v����1ms(@8MHz)���Ȥθ���z�ߤ�ʹ��)
void TIM2_interrupt_init(void);

#endif /* USER_MYLIB_TIMER_H_ */
