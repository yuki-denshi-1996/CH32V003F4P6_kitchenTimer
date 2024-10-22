/*
 * Pin.h
 *
 *  Created on: Sep 22, 2024
 *      Author: yuki(denshi-1996)
 */

#ifndef USER_TIMER_2024_KIT_H_
#define USER_TIMER_2024_KIT_H_

#include "Mylib_GPIO.h"

//PIN���x
#define SEG_A PC2
#define SEG_B PC4
#define SEG_C PD3
#define SEG_D PC7
#define SEG_E PC6
#define SEG_F PC3
#define SEG_G PD4
#define SEG_DOT PD2

#define SEG_CA1 PD5
#define SEG_CA2 PD6
#define SEG_CA3 PA1
#define SEG_CA4 PA2

#define SW_START_STOP PD0
#define SW_MIN PD1
#define SW_SEC PD7
#define SW_FAST1 PC0
#define SW_FAST2 PC1

#define BUZZER PC5

//MODE���x
#define TIME_PUSH_SET 10
#define MODE_TIMER 20
#define MODE_TIMER_STOP 25
#define MODE_TIMER_END 30
#define MODE_STOPWATCH 100
#define MODE_STOPWATCH_STOP 120
#define MODE_SLEEP 400
#define MODE_SLEEP_END 410

//a����������
//a���㥿��󥰌����Õr�g
#define CHATTERING_TIME 5

//a�ܥ����LѺ���r���LѺ���ж��r�g
#define LONG_PUSH_TIME 500

//FAST�ܥ���Τ�2000ms���LѺ���ж�
#define LONG_PUSH_TIME_FAST 2000

//�ܥ����LѺ���r�����������r�g
#define LONG_PUSH_TO_TIME 100

//Beep�����L����(�ܥ����Ѻ�����Ȥ����L��)
#define BEEP_PUSH_TIME 50

//BEEP���θߤ�
#define BEEP_FREQ 2000

//�ԥåԥåԥäΥԤ��g��
#define END_BEEP_INTERVAL 130
//�ԥåԥåԥä�ȫ��Εr�g
#define END_BEEP_LEN 1000

//FAST�ܥ����LѺ���ж��r�g
#define FAST_SW_LONG 1500

//Sleep�����Ф���r�g
//60��ǥ���`�פ�����
#define SLEEP_TIME 60000

#define FAST1_TIME_ADRESS 0x08003FC0
#define FAST2_TIME_ADRESS 0x08003FC2

void FLASH_write(volatile uint32_t Address, volatile uint16_t Data);
uint16_t FLASH_read(volatile uint32_t Address);
void FLASH_Erase(volatile uint32_t Address);

#endif /* USER_TIMER_2024_KIT_H_ */
