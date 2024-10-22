/*
 * Pin.h
 *
 *  Created on: Sep 22, 2024
 *      Author: yuki(denshi-1996)
 */

#ifndef USER_TIMER_2024_KIT_H_
#define USER_TIMER_2024_KIT_H_

#include "Mylib_GPIO.h"

//PIN定義
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

//MODE定義
#define TIME_PUSH_SET 10
#define MODE_TIMER 20
#define MODE_TIMER_STOP 25
#define MODE_TIMER_END 30
#define MODE_STOPWATCH 100
#define MODE_STOPWATCH_STOP 120
#define MODE_SLEEP 400
#define MODE_SLEEP_END 410

//aその他定数
//aチャタリング対策用時間
#define CHATTERING_TIME 5

//aボタン長押し時の長押し判定時間
#define LONG_PUSH_TIME 500

//FASTボタンのみ2000msで長押し判定
#define LONG_PUSH_TIME_FAST 2000

//ボタン長押し時の入力される時間
#define LONG_PUSH_TO_TIME 100

//Beep音の長さ　(ボタンを押したときの長さ)
#define BEEP_PUSH_TIME 50

//BEEP音の高さ
#define BEEP_FREQ 2000

//ピッピッピッのピの間隔
#define END_BEEP_INTERVAL 130
//ピッピッピッの全体の時間
#define END_BEEP_LEN 1000

//FASTボタン長押し判定時間
#define FAST_SW_LONG 1500

//Sleepに移行する時間
//60秒でスリープに移行
#define SLEEP_TIME 60000

#define FAST1_TIME_ADRESS 0x08003FC0
#define FAST2_TIME_ADRESS 0x08003FC2

void FLASH_write(volatile uint32_t Address, volatile uint16_t Data);
uint16_t FLASH_read(volatile uint32_t Address);
void FLASH_Erase(volatile uint32_t Address);

#endif /* USER_TIMER_2024_KIT_H_ */
