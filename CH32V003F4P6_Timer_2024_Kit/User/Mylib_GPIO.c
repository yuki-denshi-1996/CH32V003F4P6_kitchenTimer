/*
* SPDX-FileCopyrightText: 2024 yuki-denshi-1996 <matsukawa.software@gmail.com>
* SPDX-License-Identifier: BSD 3-Clause
*/

/*
 * CH32V003F4P6用（CH32V003)GPIO操作ライブラリ
 * Mylib_GPIO.c
 *
 *  Created on: Sep 22, 2024
 *      Author: yuki (denshi-1996)
 */

#include "Mylib_GPIO.h"
#include "stdint.h"
#include "ch32v00x.h"

//GPIO 初期化（クロック供給）
//GPIOを使う場合は、最初に有効化すること
void GPIOA_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;

    //PA1有効 (I/Oとして使用)
    AFIO->PCFR1 &= ~AFIO_PCFR1_PA12_REMAP;
}

void GPIOC_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
}

void GPIOD_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
}

//Alternate Function Enable
void AFIO_init(void){
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO;
}

//PIN event 設定関数
//pin:ピン番号 | mode:モード（EVENT_RISE and EVETN_FALL)
void eventGPIO(int pin, int mode){
    EXTI->EVENR |= (1 << (pin & 0x0F)); //Event有効化(PORTxY)
    //Rise(立ち上がり)だったら
    if(mode & EVENT_RISE){
        EXTI->RTENR |= (1 << (pin & 0x0F));
    }
    //Fall(たち下がり）だったら
    if(mode & EVENT_FALL){
        EXTI->FTENR |= (1 << (pin & 0x0F));
    }

    //PAxの場合
    if((pin & 0xF0) == 0xA0){
        AFIO->EXTICR = bit_replace(AFIO->EXTICR, 0b00, 2, (pin & 0x0F) * 2);
    }
    //PCxの場合
    else if((pin & 0xF0) == 0xC0){
        AFIO->EXTICR = bit_replace(AFIO->EXTICR, 0b10, 2, (pin & 0x0F) * 2);
    }
    //PDxの場合
    else if((pin & 0xF0) == 0xD0){
        AFIO->EXTICR = bit_replace(AFIO->EXTICR, 0b11, 2, (pin & 0x0F) * 2);
    }
}

//pin(ピン)入出力設定
//pin:ピン番| mode:モード
void pinMode(int pin, int mode) {
    //PAxポート操作
    if ((pin & 0xF0) == 0xA0) {
        //OUTPUTの場合
        if ((mode & 0b1000) == 0b0000) {
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR,
                    OUTPUT_DEFAULT_SpeedMode(0b00), 2, (pin & 0x0F) * 4);
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
        } else { //INPUTの場合
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, 0b00, 2, (pin & 0x0F) * 4);
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
            //PULLUP/PULLDOWNの判定
            if ((mode & 0b1011) == 0b1010) {
                GPIOA->OUTDR = bit_replace(GPIOA->OUTDR, (mode & 0b0100) >> 2, 1,
                        pin & 0x0F);
            }
        }
    }

    //PCxポート操作
    if ((pin & 0xF0) == 0xC0) {
        //OUTPUTの場合
        if ((mode & 0b1000) == 0b0000) {
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR,
                    OUTPUT_DEFAULT_SpeedMode(0b00), 2, (pin & 0x0F) * 4);
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
        } else { //INPUTの場合
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, 0b00, 2, (pin & 0x0F) * 4);
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
            //PULLUP/PULLDOWNの判定
            if ((mode & 0b1011) == 0b1010) {
                GPIOC->OUTDR = bit_replace(GPIOC->OUTDR, (mode & 0b0100) >> 2, 1,
                        pin & 0x0F);
            }
        }
    }

    //PDxポート操作
    if ((pin & 0xF0) == 0xD0) {
        //OUTPUTの場合
        if ((mode & 0b1000) == 0b0000) {
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR,
                    OUTPUT_DEFAULT_SpeedMode(0b00), 2, (pin & 0x0F) * 4);
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
        } else { //INPUTの場合
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, 0b00, 2, (pin & 0x0F) * 4);
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
            //PULLUP/PULLDOWNの判定
            if ((mode & 0b1011) == 0b1010) {
                GPIOD->OUTDR = bit_replace(GPIOD->OUTDR, (mode & 0b0100) >> 2, 1,
                        pin & 0x0F);
            }
        }
    }
}

//ピン出力速度指定関数
//pin:ピン番号|speed_mode:出力スピード設定
//speed_modeに00を指定した場合、そのままの値が返却される
int OUTPUT_DEFAULT_SpeedMode(int speed_mode) {
    static int mode = OUTPUT_SPEED_MODE_10MHz;
    if (speed_mode == 0b00) {
        return mode;
    }
    mode = speed_mode;
    return mode;
}

//デジタル出力設定
//pin:ピン番号|value:1(HIGH) or 0(LOW)
void digitalWrite(int pin, int value) {
    if ((pin & 0xF0) == 0xA0) {
        GPIOA->OUTDR = bit_replace(GPIOA->OUTDR, value, 1, (pin & 0x0F));
    }
    if ((pin & 0xF0) == 0xC0) {
        GPIOC->OUTDR = bit_replace(GPIOC->OUTDR, value, 1, (pin & 0x0F));
    }
    if ((pin & 0xF0) == 0xD0) {
        GPIOD->OUTDR = bit_replace(GPIOD->OUTDR, value, 1, (pin & 0x0F));
    }
}

//デジタル入力設定
//返り値：Pinの状態|Pin:ピン番号
int digitalRead(int pin) {
    if ((pin & 0xF0) == 0xA0) {
        if ((GPIOA->INDR & (1 << (pin & 0x0F))) == 0) {
            return 0;
        } else {
            return 1;
        }
    }
    if ((pin & 0xF0) == 0xC0) {
        if ((GPIOC->INDR & (1 << (pin & 0x0F))) == 0) {
            return 0;
        } else {
            return 1;
        }
    }
    if ((pin & 0xF0) == 0xD0) {
        if ((GPIOD->INDR & (1 << (pin & 0x0F))) == 0) {
            return 0;
        } else {
            return 1;
        }
    }
    return -1;
}

//  ビット置き換え関数
//  引数 data:置き換え前のビット列 | byte:置き換えるビット |
//  len:置き換えるビットの長さ | shift:シフト数
uint32_t bit_replace(uint32_t data, uint32_t byte, uint8_t len, uint8_t shift) {
    uint32_t mask = ~(((1 << len) - 1) << shift);
    data &= mask;
    data |= byte << shift;
    return data;
}
