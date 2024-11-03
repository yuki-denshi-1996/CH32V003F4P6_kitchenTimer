/*
* SPDX-FileCopyrightText: 2024 yuki-denshi-1996 <matsukawa.software@gmail.com>
* SPDX-License-Identifier: BSD 3-Clause
*/

/*
 * CH32V003F4P6�ã�CH32V003)GPIO�����饤�֥��
 * Mylib_GPIO.c
 *
 *  Created on: Sep 22, 2024
 *      Author: yuki (denshi-1996)
 */

#include "Mylib_GPIO.h"
#include "stdint.h"
#include "ch32v00x.h"

//GPIO ���ڻ�������å����o��
//GPIO��ʹ�����Ϥϡ�������Є������뤳��
void GPIOA_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;

    //PA1�Є� (I/O�Ȥ���ʹ��)
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

//PIN event �O���v��
//pin:�ԥ󷬺� | mode:��`�ɣ�EVENT_RISE and EVETN_FALL)
void eventGPIO(int pin, int mode){
    EXTI->EVENR |= (1 << (pin & 0x0F)); //Event�Є���(PORTxY)
    //Rise(�����Ϥ���)���ä���
    if(mode & EVENT_RISE){
        EXTI->RTENR |= (1 << (pin & 0x0F));
    }
    //Fall(�����¤��꣩���ä���
    if(mode & EVENT_FALL){
        EXTI->FTENR |= (1 << (pin & 0x0F));
    }

    //PAx�Έ���
    if((pin & 0xF0) == 0xA0){
        AFIO->EXTICR = bit_replace(AFIO->EXTICR, 0b00, 2, (pin & 0x0F) * 2);
    }
    //PCx�Έ���
    else if((pin & 0xF0) == 0xC0){
        AFIO->EXTICR = bit_replace(AFIO->EXTICR, 0b10, 2, (pin & 0x0F) * 2);
    }
    //PDx�Έ���
    else if((pin & 0xF0) == 0xD0){
        AFIO->EXTICR = bit_replace(AFIO->EXTICR, 0b11, 2, (pin & 0x0F) * 2);
    }
}

//pin(�ԥ�)������O��
//pin:�ԥ�| mode:��`��
void pinMode(int pin, int mode) {
    //PAx�ݩ`�Ȳ���
    if ((pin & 0xF0) == 0xA0) {
        //OUTPUT�Έ���
        if ((mode & 0b1000) == 0b0000) {
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR,
                    OUTPUT_DEFAULT_SpeedMode(0b00), 2, (pin & 0x0F) * 4);
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
        } else { //INPUT�Έ���
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, 0b00, 2, (pin & 0x0F) * 4);
            GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
            //PULLUP/PULLDOWN���ж�
            if ((mode & 0b1011) == 0b1010) {
                GPIOA->OUTDR = bit_replace(GPIOA->OUTDR, (mode & 0b0100) >> 2, 1,
                        pin & 0x0F);
            }
        }
    }

    //PCx�ݩ`�Ȳ���
    if ((pin & 0xF0) == 0xC0) {
        //OUTPUT�Έ���
        if ((mode & 0b1000) == 0b0000) {
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR,
                    OUTPUT_DEFAULT_SpeedMode(0b00), 2, (pin & 0x0F) * 4);
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
        } else { //INPUT�Έ���
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, 0b00, 2, (pin & 0x0F) * 4);
            GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
            //PULLUP/PULLDOWN���ж�
            if ((mode & 0b1011) == 0b1010) {
                GPIOC->OUTDR = bit_replace(GPIOC->OUTDR, (mode & 0b0100) >> 2, 1,
                        pin & 0x0F);
            }
        }
    }

    //PDx�ݩ`�Ȳ���
    if ((pin & 0xF0) == 0xD0) {
        //OUTPUT�Έ���
        if ((mode & 0b1000) == 0b0000) {
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR,
                    OUTPUT_DEFAULT_SpeedMode(0b00), 2, (pin & 0x0F) * 4);
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
        } else { //INPUT�Έ���
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, 0b00, 2, (pin & 0x0F) * 4);
            GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, (mode & 0b0011), 2,
                    (pin & 0x0F) * 4 + 2);
            //PULLUP/PULLDOWN���ж�
            if ((mode & 0b1011) == 0b1010) {
                GPIOD->OUTDR = bit_replace(GPIOD->OUTDR, (mode & 0b0100) >> 2, 1,
                        pin & 0x0F);
            }
        }
    }
}

//�ԥ�����ٶ�ָ���v��
//pin:�ԥ󷬺�|speed_mode:�������ԩ`���O��
//speed_mode��00��ָ���������ϡ����Τޤޤ΂�����ȴ�����
int OUTPUT_DEFAULT_SpeedMode(int speed_mode) {
    static int mode = OUTPUT_SPEED_MODE_10MHz;
    if (speed_mode == 0b00) {
        return mode;
    }
    mode = speed_mode;
    return mode;
}

//�ǥ���������O��
//pin:�ԥ󷬺�|value:1(HIGH) or 0(LOW)
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

//�ǥ����������O��
//���ꂎ��Pin��״�B|Pin:�ԥ󷬺�
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

//  �ӥå��ä��Q���v��
//  ���� data:�ä��Q��ǰ�Υӥå��� | byte:�ä��Q����ӥå� |
//  len:�ä��Q����ӥåȤ��L�� | shift:���ե���
uint32_t bit_replace(uint32_t data, uint32_t byte, uint8_t len, uint8_t shift) {
    uint32_t mask = ~(((1 << len) - 1) << shift);
    data &= mask;
    data |= byte << shift;
    return data;
}
