/*
 * CH32V003(CH32V003F4P6)用 タイマーライブラリ
 * Mylib_Timer.h
 *
 *  Created on: Sep 25, 2024
 *      Author: yuki(denshi-1996)
 */

#ifndef USER_MYLIB_TIMER_H_
#define USER_MYLIB_TIMER_H_

//Timer1をPWMとして使う設定
void TIM1_PWM_init(void);

//PWMをPC5(T1CH3_3)に接続
void tone(int freq); //TIM1(PWM)を使って音を鳴らす関数
void no_tone(void); //toneで鳴らした音を止める関数

//Timer2初期化関数（1ms(@8MHz)ごとの割り込みに使用)
void TIM2_interrupt_init(void);

#endif /* USER_MYLIB_TIMER_H_ */
