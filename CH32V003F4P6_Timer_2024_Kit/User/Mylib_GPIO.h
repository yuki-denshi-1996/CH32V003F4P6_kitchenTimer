/*
 * CH32V003F4P6用（CH32V003)GPIO操作ライブラリ
 * Mylib_GPIO.h
 *
 *  Created on: Sep 22, 2024
 *      Author: yuki (denshi-1996)
 */
#include <stdint.h>

#ifndef USER_MYLIB_GPIO_H_
#define USER_MYLIB_GPIO_H_

#define HIGH 1
#define LOW 0

//GPIO定義
#define PA1 0xA1
#define PA2 0xA2

#define PC0 0xC0
#define PC1 0xC1
#define PC2 0xC2
#define PC3 0xC3
#define PC4 0xC4
#define PC5 0xC5
#define PC6 0xC6
#define PC7 0xC7

#define PD0 0xD0
#define PD1 0xD1
#define PD2 0xD2
#define PD3 0xD3
#define PD4 0xD4
#define PD5 0xD5
#define PD6 0xD6
#define PD7 0xD7

//GPIO_MODE
#define INPUT 0b1000
#define F_INPUT 0b1001
#define INPUT_PULLUP 0b1110
#define INPUT_PULLDOWN 0b1010
#define OUTPUT 0b0000
#define OD_OUTPUT 0b0001
#define F_OUTPUT 0b0010
#define F_OD_OUTPUT 0b0011

//GPIO_OUTPUT_SPEED_MODE
#define OUTPUT_SPEED_MODE_10MHz 0b01
#define OUTPUT_SPEED_MODE_2MHz 0b10
#define OUTPUT_SPEED_MODE_50MHz 0b11

//EventGPIO
#define EVENT_RISE 0x01 //Rise(立ち上がり)でイベント発生
#define EVENT_FALL 0x02 //Fall(たち下がり)でイベント発生

//GPIO 初期化（クロック供給）
//GPIOを使う場合は、最初に有効化すること
void GPIOA_init(void);
void GPIOC_init(void);
void GPIOD_init(void);

//AFIO初期化
void AFIO_init(void);

//PINイベント設定関数
//pin:ピン | mode:立ち上がり,たち下がり　どちらか
void eventGPIO(int pin, int mode);

//ピン入出力設定
//pin:ピン番| mode:モード
void pinMode(int pin,int mode);

//ピン出力速度指定関数
//pin:ピン番号|speed_mode:出力スピード設定
int OUTPUT_DEFAULT_SpeedMode(int speed_mode);

//デジタル出力設定
//pin:ピン番号|value:1(HIGH) or 0(LOW)
void digitalWrite(int pin,int value);

//デジタル入力設定
//返り値：Pinの状態|Pin:ピン番号
int digitalRead(int pin);

//bitビット置き換え関数
uint32_t bit_replace(uint32_t data, uint32_t byte, uint8_t len, uint8_t shift);

#endif /* USER_MYLIB_GPIO_H_ */
